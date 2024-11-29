#include <strings.h>
#include "MaszynaTCPIP.h"
#include "Logger.h"

MaszynaTCPIP::MaszynaTCPIP(TCPIP *tcpip, Configuration *conf) :
	IController(IController::ControllerType::HOST_CONTROLLER), Maszyna(conf)
{
	this->tcpip = tcpip;
	tcpip->connectSocket();
}

MaszynaTCPIP::~MaszynaTCPIP()
{
}

void MaszynaTCPIP::sync(IController::SyncDirection dir)
{
	if (dir == IController::SyncDirection::FROM_CONTROLLER) {
		readTCPIP();
	} else {
		writeTCPIP();
	}
}

void MaszynaTCPIP::readTCPIP(void)
{
	uint8_t buffer[MASZYNA_INPUT_BUFFER_SIZE];
	bzero(buffer, MASZYNA_INPUT_BUFFER_SIZE);

	if (!this->tcpip->isConnected()) {
		log_error("TCPIP client not connected\n");
		return;
	}

	ssize_t read_bytes = this->tcpip->readData(buffer, MASZYNA_INPUT_BUFFER_SIZE);

	if (read_bytes != MASZYNA_INPUT_BUFFER_SIZE) {
		log_error("Received truncated buffer (%u/%u bytes)\n", read_bytes, MASZYNA_INPUT_BUFFER_SIZE);
		dumpBuffer(true, buffer, read_bytes);
		return;
	}

	handleInputBuffer(buffer, MASZYNA_INPUT_BUFFER_SIZE);
}

void MaszynaTCPIP::writeTCPIP(void)
{
	uint8_t buffer[MASZYNA_OUTPUT_BUFFER_SIZE];

	if (!this->tcpip->isConnected()) {
		log_error("TCPIP client not connected\n");
		return;
	}

	if (handleOutputBuffer(buffer, MASZYNA_OUTPUT_BUFFER_SIZE)) {
		this->tcpip->writeData(buffer, MASZYNA_OUTPUT_BUFFER_SIZE);
	}
}
