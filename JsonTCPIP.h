#ifndef JSON_TCPIP_H
#define JSON_TCPIP_H

#include "IController.h"
#include "Configuration.h"
#include "TCPIP.h"

class JsonTCPIP : public IController {
	private:
		TCPIP *tcpip;
		Configuration *conf;

		void readTCPIP(void);
		void writeTCPIP(void);

	public:
		JsonTCPIP(TCPIP *tcpip, Configuration *conf, bool dump_updates);
		virtual ~JsonTCPIP();

		virtual void sync(SyncDirection dir);
};

#endif /* JSON_TCPIP_H */
