#ifndef TCPIP_H
#define TCPIP_H

#include <string>
#include "ICommunicationHandler.h"

class TCPIP: ICommunicationHandler {
	public:
		TCPIP(const std::string & address, unsigned int port);

		virtual HandlerType getHandlerType(void);
		virtual ssize_t readData(uint8_t *buffer, size_t length);
		virtual ssize_t writeData(const uint8_t *buffer, size_t length);
};

#endif /* TCPIP_H */
