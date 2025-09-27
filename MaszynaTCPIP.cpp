#include <strings.h>
#include <string.h>
#include "MaszynaTCPIP.h"
#include "Logger.h"

#define MASZYNA_TCP_PREAMBLE_BYTES_COUNT 4
#define MASZYNA_TCP_PREAMBLE_BYTE 0xEF

MaszynaTCPIP::MaszynaTCPIP(TCPIP *tcpip, Configuration *conf, bool dump_updates) :
	IController(IController::ControllerType::HOST_CONTROLLER, dump_updates), Maszyna(conf, dump_updates)
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

unsigned int MaszynaTCPIP::getPreambleOffset(uint8_t *buffer, unsigned int buffer_size)
{
	unsigned int result = (unsigned int) -1;

	if (buffer_size >= MASZYNA_TCP_PREAMBLE_BYTES_COUNT) {
		for (unsigned int i = 0; i < buffer_size - MASZYNA_TCP_PREAMBLE_BYTES_COUNT + 1; i++) {
			if (((buffer[i] & 0xff) == MASZYNA_TCP_PREAMBLE_BYTE) &&
					((buffer[i + 1] & 0xff) == MASZYNA_TCP_PREAMBLE_BYTE) &&
					((buffer[i + 2] & 0xff) == MASZYNA_TCP_PREAMBLE_BYTE) &&
					((buffer[i + 3] & 0xff) == MASZYNA_TCP_PREAMBLE_BYTE)) {
				result = i;
				break;
			}
		}
	}

	if ((result != (unsigned int) -1) && (result > 0)) {
		log_warning("Preamble found at index %u / %u\n", result, buffer_size);
	}

	return result;
}

unsigned int MaszynaTCPIP::readPreamble(uint8_t *buffer, unsigned int buffer_size)
{
	unsigned int bytes_read = 0;
	unsigned int preamble_offset = (unsigned int) -1;
	unsigned int data_read = 0;

	while ((bytes_read + MASZYNA_TCP_PREAMBLE_BYTES_COUNT <= buffer_size) && (preamble_offset == (unsigned int) -1)) {
		bytes_read += this->tcpip->readData(&buffer[bytes_read], MASZYNA_TCP_PREAMBLE_BYTES_COUNT);
		preamble_offset = getPreambleOffset(buffer, bytes_read);
	}

	if (preamble_offset != (unsigned int) -1) {
		data_read = bytes_read - preamble_offset;

		if (preamble_offset != 0) {
			memmove(&buffer[0], &buffer[preamble_offset], data_read);
		}
	} else {
		log_error("Could not find preamble !!!\n");
	}

	return data_read;
}

void MaszynaTCPIP::readTCPIP(void)
{
	uint8_t buffer[MASZYNA_INPUT_BUFFER_SIZE];
	bzero(buffer, MASZYNA_INPUT_BUFFER_SIZE);

	if (!this->tcpip->isConnected()) {
		log_error("TCPIP client not connected\n");
		return;
	}

	unsigned int bytes_read = readPreamble(buffer, MASZYNA_INPUT_BUFFER_SIZE);

	if (bytes_read == 0)
		return;

	bytes_read += this->tcpip->readData(&buffer[bytes_read], MASZYNA_INPUT_BUFFER_SIZE - bytes_read);

	if (bytes_read != MASZYNA_INPUT_BUFFER_SIZE) {
		log_error("Received truncated buffer (%u/%u bytes)\n", bytes_read, MASZYNA_INPUT_BUFFER_SIZE);
		dumpBuffer(true, buffer, bytes_read);
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
