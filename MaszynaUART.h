#ifndef MASZYNA_UART_H
#define MASZYNA_UART_H

#include "IController.h"
#include "ICommunicationHandler.h"
#include "Configuration.h"

class MaszynaUART : public IController {
	private:
		ICommunicationHandler *uart;
		Configuration *conf;
		bool send_packet_if_received;
		bool packet_received;

		unsigned int getConfigValue(unsigned int id);
	public:
		MaszynaUART(ICommunicationHandler *uart, Configuration *conf, bool send_if_received);
		virtual ~MaszynaUART();

		void readUART(void);
		void writeUART(void);

		virtual void sync(SyncDirection dir);
		void dumpBuffer(uint8_t *buffer, ssize_t size);
};

#endif /* MASZYNA_UART_H */
