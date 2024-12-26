#include <strings.h>
#include <string.h>
#include "VirtualControlTCPIP.h"
#include "DataMessage.h"
#include "Logger.h"

VirtualControlTCPIP::VirtualControlTCPIP(TCPIP *tcpip, Configuration *conf) :
	IController(IController::ControllerType::HOST_CONTROLLER)
{
	this->tcpip = tcpip;
	this->conf = conf;

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

	if (!this->tcpip->isConnected()) {
		log_error("TCPIP client not connected\n");
		return;
	}

	this->conf->accessLock();

	if (this->conf->checkUpdates(this->configuration_ids, VIRTUAL_CONTROL_TCPIP_CONFIGURATION_IDS_COUNT)) {
		dataMessage = new DataMessage(DataMessage::MessageType::MESSAGE_TYPE_SEND_DATA);

		for (uint32_t i = 0; i < VIRTUAL_CONTROL_TCPIP_CONFIGURATION_IDS_COUNT; i++) {
			if (this->conf->isUpdated(this->configuration_ids[i])) {
				dataMessage->addMessageItem(this->configuration_ids[i], this->conf->getValue(this->configuration_ids[i]));
				this->conf->cleanUpdates(this->configuration_ids[i]);
			}
		}
	}

	this->conf->accessUnlock();

	if (dataMessage) {
		size_t size;
		uint8_t *buffer = dataMessage->getRawData(size);

		this->tcpip->writeData(buffer, size);
		delete dataMessage;
	}
}
