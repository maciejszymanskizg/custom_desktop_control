#ifndef ICOMMUNICATION_HANDLER_H
#define ICOMMUNICATION_HANDLER_H

#include <stdint.h>
#include <sys/types.h>

class ICommunicationHandler {
	public:
		enum HandlerType {
			COMMUNICATION_HANDLER_I2C = 0,
			COMMUNICATION_HANDLER_UART,
			COMMUNICATION_HANDLER_TCPIP
		};

		virtual ~ICommunicationHandler() = default;
		virtual ssize_t readData(uint8_t *buffer, size_t size) = 0;
		virtual ssize_t writeData(const uint8_t *buffer, size_t size) = 0;
		virtual HandlerType getHandlerType(void) = 0;


};

#endif /* COMMUNICATION_HANDLER_H */
