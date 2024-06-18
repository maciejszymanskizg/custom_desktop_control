#include <errno.h>
#include <string.h>
#include "PCF8575.h"
#include "Logger.h"

PCF8575::PCF8575(ICommunicationHandler *ch, uint8_t a0, uint8_t a1, uint8_t a2)
{
	if (ch == nullptr) {
		log_error("Invalid CommunicationHandler object.\n");
		return;
	}

	if (ch->getHandlerType() != ICommunicationHandler::HandlerType::COMMUNICATION_HANDLER_I2C) {
		log_error("I2C type CommunicationHandel required.\n");
		return;
	}

	addr = (uint8_t) 0x20 | (!!a0 << 1) | (!!a1 << 2) | (!!a2 << 3);
	i2c = dynamic_cast<I2C *>(ch);
}


PCF8575::~PCF8575()
{

}

void PCF8575::setup(const uint16_t & _mask)
{
	int res;

	if (!mask) {
		mask = _mask;
	}

	log_debug("Setting mask 0x%x on I2c device 0x%x\n", mask, addr);
	res = i2c->setSlaveAddr(addr);
	if (res != 0) {
		return;
	}

	res = i2c->writeData((const uint8_t *) &mask, sizeof(mask));
	if (res == -1) {
		log_error("Error in writing mask to I2c device : %s\n", strerror(errno));
	}

	value = _mask;
}

int PCF8575::writeData(const uint16_t & data)
{
	int res = 0;
	uint16_t to_write = data & mask;

	if (i2c->setSlaveAddr(addr) != 0) {
		res = -1;
	} else {
		ssize_t written = i2c->writeData((const uint8_t *) &to_write, sizeof(to_write));

		if (written == -1) {
			log_error("Could not write data to I2C device : %s\n", strerror(errno));
			res = -1;
		}
	}

	return res;
}

int PCF8575::readData(uint16_t & data)
{
	int res = 0;

	if (i2c->setSlaveAddr(addr) != 0) {
		res = -1;
	} else {
		ssize_t read_data = i2c->readData((uint8_t *) &data, sizeof(data));

		if (read_data == -1) {
			log_error("Could not read data from I2C device : %s\n", strerror(errno));
			res = -1;
		}
	}

	return res;
}

int PCF8575::shiftFromPin(Pin pin)
{
	return static_cast<int>(pin);
}

int PCF8575::sync(PCF8575::SyncDirection direction)
{
	int res = 0;

	if (direction == SyncDirection::SYNC_FROM_PCF8575) {
		res = readData(value);
	} else {
		res = writeData(value);
	}

	return res;
}

PCF8575::PinState PCF8575::getInput(PCF8575::Pin pin)
{
	PinState result = PinState::PinStateLow;

	if (mask & (1 << shiftFromPin(pin))) {
		result = static_cast<PinState>((value >> shiftFromPin(pin)) & 0x1);
	} else {
		log_error("Requested bit is not configured as input");
	}

	return result;
}

void PCF8575::setOutput(Pin pin, PinState state)
{
	if (mask & (1 << shiftFromPin(pin))) {
		log_error("Requested bit is not configured as output");
	} else {
		value &= ~(1 << shiftFromPin(pin));
		if (state == PinStateHigh) {
			value |= (1 << shiftFromPin(pin));
		}
	}
}
