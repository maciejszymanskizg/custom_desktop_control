#ifndef VIRT_EU07_H
#define VIRT_EU07_H

#include <atomic>
#include "IController.h"
#include "Configuration.h"
#include "TCPIP.h"
#include "DataMessage.h"

class VirtEU07 : public IController {
	private:
		Configuration *conf;
		TCPIP *tcpip;
		std::atomic<bool> connected;

		DataMessage *createRequestDataMessage(void);
		DataMessage *receiveDataMessage(void);
		void handleInput(void);
		void handleOutput(void);

	public:
		VirtEU07(Configuration *conf, TCPIP *tcpip);
		virtual ~VirtEU07();

		virtual void sync(SyncDirection dir);
		void waitForConnection(void);
};

#endif /* VIRT_EU07_H */
