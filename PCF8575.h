#ifndef PCF8575_H
#define PCF8575_H

#include <stdint.h>
#include "I2C.h"

class PCF8575 {
	public:
		enum SyncDirection {
			SYNC_FROM_PCF8575 = 0,
			SYNC_TO_PCF8575
		};

		enum Pin {
			PIN_P0 = 0,
			PIN_P1,
			PIN_P2,
			PIN_P3,
			PIN_P4,
			PIN_P5,
			PIN_P6,
			PIN_P7,
			PIN_P10,
			PIN_P11,
			PIN_P12,
			PIN_P13,
			PIN_P14,
			PIN_P15,
			PIN_P16,
			PIN_P17
		};

		enum PinState {
			PinStateLow = 0,
			PinStateHigh
		};

		PCF8575(ICommunicationHandler *ch, uint8_t a0, uint8_t a1, uint8_t a2);
		~PCF8575();

		void setup(const uint16_t & mask);
		int sync(SyncDirection direction);
		PinState getInput(Pin pin);
		void setOutput(Pin pin, PinState state);

	private:
		I2C *i2c;
		uint16_t mask;
		uint8_t addr;
		uint16_t value;

		int setAddr(void);
		int readData(uint16_t & data);
		int writeData(const uint16_t & data);
		int shiftFromPin(Pin pin);
};

#endif /* PCF8575_H */
