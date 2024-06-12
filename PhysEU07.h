#ifndef PHYS_EU07_H
#define PHYS_EU07_H

#include "IController.h"
#include "Configuration.h"
#include "I2C.h"

class PhysEU07 : public IController {
	private:
		Configuration *conf;
		I2C *i2c;

		void configToI2C(void);
		void I2CToConfig(void);

		void prepareDataBuffer(uint8_t *buffer, uint32_t size, uint32_t value1, uint32_t value2);

		void read_0x40(void);
		void write_0x42(void);
		void write_0x43(void);
	public:
		PhysEU07(Configuration *conf, ICommunicationHandler *handler);
		virtual ~PhysEU07();

		virtual void sync(SyncDirection dir);
};

#endif /* PHYS_EU07_H */
