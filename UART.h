#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "ICommunicationHandler.h"
#include "FDOperations.h"

class UART : public ICommunicationHandler, FDOperations {
	public:
		enum Baudrate {
			BAUDRATE_9600,
			BAUDRATE_19200,
			BAUDRATE_38400,
			BAUDRATE_57600,
			BAUDRATE_115200,
			BAUDRATE_230400,
			BAUDRATE_460800
		};

		UART(const char *device, Baudrate baudrate, volatile sig_atomic_t *running_flag);
		virtual ~UART();

		virtual HandlerType getHandlerType(void);
		virtual ssize_t readData(uint8_t *buffer, size_t size);
		virtual ssize_t writeData(const uint8_t *buffer, size_t size);

	private:
		int fd;
};

#endif /* UART_H */
