#ifndef MASZYNA_UART_H
#define MASZYNA_UART_H

#include "IController.h"
#include "ICommunicationHandler.h"
#include "Configuration.h"

class MaszynaUART : public IController {
	private:
		ICommunicationHandler *uart;
		Configuration *conf;

		unsigned int getConfigValue(unsigned int id);
	public:
		MaszynaUART(ICommunicationHandler *uart, Configuration *conf);
		~MaszynaUART();

		void readUART(void);
		void writeUART(void);

		virtual void sync(SyncDirection dir);
};

#endif /* MASZYNA_UART_H */
