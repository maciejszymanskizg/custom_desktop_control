#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "TCPIP.h"
#include "Logger.h"

TCPIP::TCPIP(enum TCPIP::Mode mode, const std::string & address, unsigned int port)
{
	this->mode = mode;
	this->address = address;
	this->port = port;
	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	this->client_sockfd = -1;
	this->connected = false;

	if (this->sockfd == -1) {
		log_error("Could not create socket.\n");
	}
}

TCPIP::~TCPIP()
{
	if (this->sockfd != -1)
		close(this->sockfd);

	if (this->client_sockfd != -1)
		close(this->client_sockfd);
}

bool TCPIP::connectSocket(void)
{
	struct sockaddr_in addr, cli;
	socklen_t len = sizeof(cli);

	if (this->sockfd == -1) {
		log_error("Invalid socket.\n");
		goto err;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(this->address.c_str());
	addr.sin_port = htons(this->port);

	if (mode == TCPIP_MODE_SERVER) {
		int enable = 1;

		if (setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)) != 0) {
			log_error("Could not change socket options - errno %d\n", errno);
			goto err;
		}

		if (bind(this->sockfd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
			log_error("Could not bind socket - errno %d\n", errno);
			goto err;
		}

		if (listen(this->sockfd, 5) != 0) {
			log_error("Could not listen on socket - errno %d\n", errno);
			goto err;
		}

		this->client_sockfd = accept(this->sockfd, (struct sockaddr *) &cli, &len);

		if (this->client_sockfd < 0) {
			log_error("Could not accept connection - errno %d\n", errno);
			goto err;
		} else {
			log_info("Connection with %s on port %u established (client sock_fd %d).\n", inet_ntoa(cli.sin_addr),
					ntohs(cli.sin_port), this->client_sockfd);
			this->connected = true;
		}
	} else {
		if (connect(this->sockfd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
			log_error("Could not connect with %s:%u - errno %d\n", inet_ntoa(addr.sin_addr),
					ntohl(addr.sin_port));
			goto err;
		} else {
			log_info("Successfully connected to %s:%d.\n", inet_ntoa(addr.sin_addr),
					ntohl(addr.sin_port));
			this->connected = true;
		}
	}

	return true;

err:
	if (this->sockfd != -1) {
		close(this->sockfd);
		this->sockfd = -1;
	}

	return false;
}

ICommunicationHandler::HandlerType TCPIP::getHandlerType(void)
{
	return ICommunicationHandler::HandlerType::COMMUNICATION_HANDLER_TCPIP;
}

ssize_t TCPIP::readData(uint8_t *buffer, size_t length)
{
	int fd = (this->mode == TCPIP_MODE_SERVER ? this->client_sockfd : this->sockfd);
	ssize_t result = readFDData(fd, buffer, length);

	if (result == 0) {
		/* Connection closed by peer */
		this->connected = false;
	}

	return result;
}

ssize_t TCPIP::writeData(const uint8_t *buffer, size_t length)
{
	int fd = (this->mode == TCPIP_MODE_SERVER ? this->client_sockfd : this->sockfd);
	ssize_t result = writeFDData(fd, buffer, length);

	if (result == 0) {
		/* Connection closed by peer */
		this->connected = false;
	}

	return result;
}

bool TCPIP::isConnected(void)
{
	return this->connected;
}
