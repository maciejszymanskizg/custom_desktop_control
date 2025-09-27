#ifndef MASZYNA_TCPIP_H
#define MASZYNA_TCPIP_H

#include "IController.h"
#include "TCPIP.h"
#include "Maszyna.h"

class MaszynaTCPIP : public IController, public Maszyna {
	private:
		TCPIP *tcpip;

		unsigned int readPreamble(uint8_t *buffer, unsigned int buffer_size);
		unsigned int getPreambleOffset(uint8_t *buffer, unsigned int buffer_size);

		void readTCPIP(void);
		void writeTCPIP(void);

	public:
		MaszynaTCPIP(TCPIP *tcpip, Configuration *conf, bool dump_updates);
		virtual ~MaszynaTCPIP();

		virtual void sync(SyncDirection dir);
};

#endif /* MASZYNA_TCPIP_H */
