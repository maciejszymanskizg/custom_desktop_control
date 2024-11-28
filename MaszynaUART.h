#ifndef MASZYNA_UART_H
#define MASZYNA_UART_H

#include "IController.h"
#include "ICommunicationHandler.h"
#include "Maszyna.h"

class MaszynaUART : public IController, public Maszyna {
	private:
		ICommunicationHandler *uart;
		bool send_packet_if_received;
		bool packet_received;

		void readUART(void);
		void writeUART(void);

	public:
		MaszynaUART(ICommunicationHandler *uart, Configuration *conf, bool send_if_received);
		virtual ~MaszynaUART();

		virtual void sync(SyncDirection dir);
};

#endif /* MASZYNA_UART_H */
