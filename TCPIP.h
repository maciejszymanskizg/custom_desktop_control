#ifndef TCPIP_H
#define TCPIP_H

#include <string>
#include "ICommunicationHandler.h"
#include "FDOperations.h"

class TCPIP : public ICommunicationHandler, FDOperations {
	public:
		enum Mode {
			TCPIP_MODE_SERVER,
			TCPIP_MODE_CLIENT
		};

		TCPIP(enum Mode mode, const std::string & address, unsigned int port);
		virtual ~TCPIP();
		bool connectSocket(void);

		virtual HandlerType getHandlerType(void);
		virtual ssize_t readData(uint8_t *buffer, size_t length);
		virtual ssize_t writeData(const uint8_t *buffer, size_t length);

	private:
		enum Mode mode;
		int sockfd;
		int client_sockfd;
		std::string address;
		unsigned int port;
};

#endif /* TCPIP_H */
