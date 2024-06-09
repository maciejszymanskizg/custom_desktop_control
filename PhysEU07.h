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

		void read_0x40(void);
		void write_0x42(void);
	public:
		PhysEU07(Configuration *conf, ICommunicationHandler *handler);
		virtual ~PhysEU07();

		virtual void sync(SyncDirection dir);
};

#endif /* PHYS_EU07_H */
