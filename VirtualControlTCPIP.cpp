#include <strings.h>
#include <string.h>
#include "VirtualControlTCPIP.h"
#include "DataMessage.h"
#include "Logger.h"

VirtualControlTCPIP::VirtualControlTCPIP(TCPIP *tcpip, Configuration *conf, bool dump_updates) :
	IController(IController::ControllerType::HOST_CONTROLLER, dump_updates)
{
	this->tcpip = tcpip;
	this->conf = conf;
	memset(this->configuration_values, 0, sizeof(this->configuration_values));

	tcpip->connectSocket();
}

VirtualControlTCPIP::~VirtualControlTCPIP()
{
}

void VirtualControlTCPIP::sync(IController::SyncDirection dir)
{
	if (dir == IController::SyncDirection::TO_CONTROLLER) {
		writeTCPIP();
	}
}

void VirtualControlTCPIP::writeTCPIP(void)
{
	DataMessage *dataMessage = nullptr;
	uint32_t value;

	if (!this->tcpip->isConnected()) {
		log_error("TCPIP client not connected\n");
		return;
	}

	dataMessage = new DataMessage(DataMessage::MessageType::MESSAGE_TYPE_SEND_DATA);

	this->conf->accessLock();

	for (uint32_t i = 0; i < VIRTUAL_CONTROL_TCPIP_CONFIGURATION_IDS_COUNT; i++) {
		value = this->conf->getValue(this->configuration_ids[i]);

		if (this->configuration_values[i] != value) {
			dataMessage->addMessageItem(this->configuration_ids[i], value);
			this->configuration_values[i] = value;
		}
	}

	this->conf->accessUnlock();

	if (dataMessage->getMessageItemsCount() > 0) {
		size_t size;
		uint8_t *buffer = dataMessage->getRawData(size);

		this->tcpip->writeData(buffer, size);
	}

	delete dataMessage;
}
