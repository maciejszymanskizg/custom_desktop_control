#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include "ICommunicationHandler.h"

class I2C : public ICommunicationHandler
{
	private:
		int fd;
		bool use_smbus;

		ssize_t readSMBusData(uint8_t *buffer, size_t length);
		ssize_t writeSMBusData(const uint8_t *buffer, size_t length);

	public:
		I2C(const std::string & path);
		virtual ~I2C();

		int setSlaveAddr(uint8_t addr);
		void setSMBus(bool enable);

		virtual HandlerType getHandlerType(void);
		virtual ssize_t readData(uint8_t *buffer, size_t length);
		virtual ssize_t writeData(const uint8_t *buffer, size_t length);
};

#endif /* I2C_H */
