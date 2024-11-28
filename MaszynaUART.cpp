#include <string.h>
#include "Logger.h"
#include "MaszynaUART.h"

MaszynaUART::MaszynaUART(ICommunicationHandler *uart, Configuration *conf, bool send_if_received) :
	IController(IController::ControllerType::HOST_CONTROLLER), Maszyna(conf)
{
	this->uart = uart;
	this->packet_received = false;
	this->send_packet_if_received = send_if_received;
}

MaszynaUART::~MaszynaUART()
{
}

void MaszynaUART::sync(IController::SyncDirection dir)
{
	if (dir == IController::SyncDirection::FROM_CONTROLLER) {
		readUART();
	} else if ((!send_packet_if_received) || (packet_received)) {
		writeUART();
	}
}

void MaszynaUART::readUART(void)
{
	uint8_t buffer[MASZYNA_INPUT_BUFFER_SIZE];
	bzero(buffer, MASZYNA_INPUT_BUFFER_SIZE);
	ssize_t read_bytes = this->uart->readData(buffer, MASZYNA_INPUT_BUFFER_SIZE);

	if (read_bytes != MASZYNA_INPUT_BUFFER_SIZE) {
		log_error("Received truncated buffer (%u/%u bytes)\n", read_bytes, MASZYNA_INPUT_BUFFER_SIZE);
		dumpBuffer(true, buffer, read_bytes);
		return;
	}

	this->packet_received = true;

	handleInputBuffer(buffer, MASZYNA_INPUT_BUFFER_SIZE);
}

void MaszynaUART::writeUART(void)
{
	uint8_t buffer[MASZYNA_OUTPUT_BUFFER_SIZE];

	if (handleOutputBuffer(buffer, MASZYNA_OUTPUT_BUFFER_SIZE)) {
		uart->writeData(buffer, MASZYNA_OUTPUT_BUFFER_SIZE);
		packet_received = false;
	}
}
