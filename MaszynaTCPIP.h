#ifndef MASZYNA_TCPIP_H
#define MASZYNA_TCPIP_H

#include "IController.h"
#include "TCPIP.h"
#include "Maszyna.h"

class MaszynaTCPIP : public IController, public Maszyna {
	private:
		TCPIP *tcpip;

		void readTCPIP(void);
		void writeTCPIP(void);

	public:
		MaszynaTCPIP(TCPIP *tcpip, Configuration *conf);
		virtual ~MaszynaTCPIP();

		virtual void sync(SyncDirection dir);
};

#endif /* MASZYNA_TCPIP_H */
