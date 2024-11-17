#ifndef PHYS_EU07_H
#define PHYS_EU07_H

#include "IController.h"
#include "Configuration.h"
#include "PCF8575.h"
#include "I2C.h"

class PhysEU07 : public IController {
	private:
		Configuration *conf;
		Configuration *global_conf;
		I2C *i2c;
		PCF8575 *pcf8575_0x20;
		PCF8575 *pcf8575_0x21;
		PCF8575 *pcf8575_0x22;
		PCF8575 *pcf8575_0x23;
		PCF8575 *pcf8575_0x24;

		void configToI2C(void);
		void I2CToConfig(void);

		void prepareDataBuffer(uint8_t *buffer, uint32_t size, uint32_t value1, uint32_t value2);
		void prepareDataBuffer(uint8_t *buffer, uint32_t size, uint32_t value1, uint32_t value2, uint32_t value3);

		void read_0x21(void);
		void read_0x22(void);
		void read_0x24(void);
		void read_0x40(void);
		void read_0x41(void);

		void write_0x20(void);
		void write_0x23(void);
		void write_0x41(void);
		void write_0x42(void);
		void write_0x43(void);
		void write_0x44(void);
		void write_0x45(void);
	public:
		PhysEU07(Configuration *train_conf, Configuration *global_conf, ICommunicationHandler *handler);
		virtual ~PhysEU07();

		virtual void sync(SyncDirection dir);
};

#endif /* PHYS_EU07_H */
