#ifndef TCPIP_H
#define TCPIP_H

#include "ICommunicationHandler.h"
#include "FDOperations.h"

class TCPIP : public ICommunicationHandler, FDOperations {
	public:
		enum Mode {
			TCPIP_MODE_SERVER,
			TCPIP_MODE_CLIENT
		};

		TCPIP(enum Mode mode, const char *address, unsigned int port, volatile sig_atomic_t *running_flag);
		virtual ~TCPIP();
		bool connectSocket(void);
		void setBlockingMode(bool blocking);

		virtual HandlerType getHandlerType(void);
		virtual ssize_t readData(uint8_t *buffer, size_t length);
		virtual ssize_t writeData(const uint8_t *buffer, size_t length);

		bool isConnected(void);

	private:
		enum Mode mode;
		int sockfd;
		int client_sockfd;
		const char *address;
		unsigned int port;
		bool bound;
		bool connected;
};

#endif /* TCPIP_H */
