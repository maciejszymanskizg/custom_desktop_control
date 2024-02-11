#include <string.h>
#include "VirtEU07.h"
#include "TrainConfigurationIDs.h"
#include "Logger.h"

VirtEU07::VirtEU07(Configuration *conf, TCPIP *tcpip) : IController(ControllerType::PHYS_CONTROLLER)
{
	this->conf = conf;
	this->tcpip = tcpip;
	this->connected = tcpip->connectSocket();
}

VirtEU07::~VirtEU07()
{

}

void VirtEU07::waitForConnection(void)
{
	this->connected = tcpip->connectSocket();
}

void VirtEU07::dumpBuffer(uint8_t *buffer, size_t size, bool inc)
{
	log_info("%s ", inc ? "<--" : "-->");
	for (size_t i = 0; i < size; i++) {
		log_info("%02x ", buffer[i] & 0xff);
	}
	log_info("\n");
}

DataMessage *VirtEU07::createRequestDataMessage(void)
{
	DataMessage *result = nullptr;

	const uint32_t update_ids[] = {
		CONFIGURATION_ID_HASLER_VELOCITY,
		CONFIGURATION_ID_VOLTMETER_LOW_VOLTAGE,
		CONFIGURATION_ID_VOLTMETER_HIGH_VOLTAGE,
		CONFIGURATION_ID_AMMETER_LOW_VOLTAGE,
		CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE1,
		CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE2,
		CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE3,
		CONFIGURATION_ID_TANK_PRESSURE,
		CONFIGURATION_ID_BREAK_PRESSURE,
		CONFIGURATION_ID_PIPE_PRESSURE,
		CONFIGURATION_ID_INDICATOR_SHP,
		CONFIGURATION_ID_INDICATOR_ALERTER,
		CONFIGURATION_ID_INDICATOR_COMPRESSOR_OVERLOAD,
		CONFIGURATION_ID_INDICATOR_VENTILATOR_OVERLOAD,
		CONFIGURATION_ID_INDICATOR_LINE_BREAKER,
		CONFIGURATION_ID_INDICATOR_TRACTION_ENGINE_OVERLOAD,
		CONFIGURATION_ID_INDICATOR_MAIN_CIRCUIT_DIFFERENTIAL,
		CONFIGURATION_ID_INDICATOR_CONVERTER_OVERLOAD,
		CONFIGURATION_ID_INDICATOR_LINE_CONTACTORS,
		CONFIGURATION_ID_INDICATOR_WHEELSLIP,
		CONFIGURATION_ID_INDICATOR_HIGH_START,
		CONFIGURATION_ID_INDICATOR_RESTISTOR_RIDE,
		CONFIGURATION_ID_INDICATOR_TRAIN_HEATING
	};

	if (this->conf->checkUpdates(update_ids, sizeof(update_ids) / sizeof(update_ids[0]))) {
		result = new DataMessage(DataMessage::MessageType::MESSAGE_TYPE_SEND_DATA);

		for (uint32_t i = 0; i < sizeof(update_ids) / sizeof(update_ids[0]); i++) {
			if (this->conf->isUpdated(update_ids[i])) {
				result->addMessageItem(update_ids[i], this->conf->getValue(update_ids[i]));
				this->conf->cleanUpdates(update_ids[i]);
			}
		}
	}

	return result;
}

DataMessage *VirtEU07::receiveDataMessage(void)
{
	DataMessage *result = nullptr;
	uint8_t header_buffer[sizeof(DataMessage::MessageHeader)];

	memset(&header_buffer, 0, sizeof(DataMessage::MessageHeader));

	if ((size_t) this->tcpip->readData(header_buffer, sizeof(DataMessage::MessageHeader)) ==
			sizeof(DataMessage::MessageHeader)) {
		DataMessage::MessageHeader *header = (DataMessage::MessageHeader *) &header_buffer;

		if (header->number_of_items > 0) {
			size_t payload_size = header->number_of_items * sizeof(DataMessage::MessageItem);
			size_t buffer_size = sizeof(DataMessage::MessageHeader) + payload_size;
			uint8_t *buffer = new uint8_t[buffer_size];

			if (buffer != nullptr) {
				memset(buffer, 0, buffer_size);
				memcpy(buffer, header_buffer, sizeof(DataMessage::MessageHeader));

				if ((size_t) this->tcpip->readData(&buffer[sizeof(DataMessage::MessageHeader)], payload_size) == payload_size) {
					result = new DataMessage(buffer, buffer_size);
				} else {
					log_error("Could not receive message payload.\n");
				}

				delete [] buffer;
			} else {
				log_error("Memory allocation error.\n");
			}
		} else {
			result = new DataMessage(header_buffer, sizeof(DataMessage::MessageHeader));
		}
	} else {
		log_error("Could not receive message header.\n");
	}

	return result;
}

void VirtEU07::handleInput(void)
{
	DataMessage *request = new DataMessage(DataMessage::MessageType::MESSAGE_TYPE_REQUEST_DATA);

	if (request == nullptr) {
		log_error("Error in creating request DataMessage.\n");
		return;
	}

	size_t size;
	uint8_t *buffer = request->getRawData(size);

	if (buffer != nullptr) {
		if (this->tcpip->writeData(buffer, size) == (ssize_t) size) {
			DataMessage *response = receiveDataMessage();

			if (response != nullptr) {
				if (response->getMessageType() == DataMessage::MessageType::MESSAGE_TYPE_RESPONSE_DATA) {
					std::vector<struct DataMessage::MessageItem *> message_items = response->getMessageItems();

					for (uint32_t i = 0; i < message_items.size(); i++) {
						this->conf->setValue(message_items[i]->id, message_items[i]->value);
					}
				} else {
					log_error("Invalid message type 0x%x\n", (uint32_t) response->getMessageType());
				}

				delete response;
			}
		}

		delete [] buffer;
	}

	delete request;
}

void VirtEU07::handleOutput(void)
{
	DataMessage *request = createRequestDataMessage();

	if (request != nullptr) {
		size_t size;
		uint8_t *buffer = request->getRawData(size);

		if (buffer != nullptr) {
			if (this->tcpip->writeData(buffer, size) == (ssize_t) size) {
				DataMessage *response = receiveDataMessage();

				if (response != nullptr) {
					if (response->getMessageType() == DataMessage::MessageType::MESSAGE_TYPE_CONFIRM_DATA) {
					} else {
						log_error("Invalid message type 0x%x\n", (uint32_t) response->getMessageType());
					}

					delete response;
				}
			}

			delete [] buffer;
		}

		delete request;
	}
}

void VirtEU07::sync(IController::SyncDirection direction)
{
	this->conf->accessLock();

	if (direction == IController::SyncDirection::FROM_CONTROLLER) {
		handleInput();
	} else {
		handleOutput();
	}

	this->conf->accessUnlock();
}
