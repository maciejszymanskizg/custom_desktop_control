#include <string.h>
#include <strings.h>
#include "JsonTCPIP.h"
#include "JSON.h"
#include "Logger.h"

#define JSON_TCPIP_BUFFER_SIZE 4096

JsonTCPIP::JsonTCPIP(TCPIP *tcpip, Configuration *conf, bool dump_updates) :
	IController(IController::ControllerType::HOST_CONTROLLER, dump_updates)
{
	this->conf = conf;
	this->tcpip = tcpip;
	tcpip->setBlockingMode(false);
}

JsonTCPIP::~JsonTCPIP()
{
}

void JsonTCPIP::sync(IController::SyncDirection dir)
{
	if (!this->tcpip->isConnected()) {
		log_debug("Trying to connect...\n");
		tcpip->connectSocket();
	}

	if (dir == IController::SyncDirection::FROM_CONTROLLER) {
		readTCPIP();
	} else {
		writeTCPIP();
	}
}

void JsonTCPIP::readTCPIP(void)
{
	ssize_t bytes_read;
	uint8_t buffer[JSON_TCPIP_BUFFER_SIZE];
	bzero(buffer, JSON_TCPIP_BUFFER_SIZE);

	/*
	if (!this->tcpip->isConnected()) {
		log_error("TCPIP client not connected\n");
		return;
	}
	*/

	bytes_read = this->tcpip->readData(buffer, JSON_TCPIP_BUFFER_SIZE);

	if (bytes_read > 2)
	{
		/* add NULL terminator at the end of buffer */
		buffer[bytes_read] = '\0';
		/* and try to parse */

		conf->accessLock();
		JSON::JsonToConfiguration((const char *) buffer, conf);
		conf->accessUnlock();
	}
}

void JsonTCPIP::writeTCPIP(void)
{
	uint8_t buffer[JSON_TCPIP_BUFFER_SIZE];
	bzero(buffer, JSON_TCPIP_BUFFER_SIZE);

	/*
	if (!this->tcpip->isConnected()) {
		log_error("TCPIP client not connected\n");
		return;
	}
	*/

	conf->accessLock();
	if (dump_updates)
		conf->dumpConfigUpdates();

	if (JSON::ConfigurationToJson(conf, (char *) buffer, JSON_TCPIP_BUFFER_SIZE))
	{
		unsigned int size = strlen((char *) buffer);
		if (size > 2)
		{
			//log_debug((char *) buffer);
			this->tcpip->writeData(buffer, size);
		}
	}

	conf->accessUnlock();

}
