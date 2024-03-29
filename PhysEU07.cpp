#include "PhysEU07.h"
#include "TrainConfigurationIDs.h"
#include "Logger.h"

#define DIFF(x, y) (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))

PhysEU07::PhysEU07(Configuration *conf, ICommunicationHandler *handler) : IController(ControllerType::PHYS_CONTROLLER)
{
	this->conf = conf;
	this->i2c = dynamic_cast<I2C *>(handler);
	if (this->i2c == nullptr) {
		log_error("Invalid communication handler.\n");
	}
}

PhysEU07::~PhysEU07()
{
}

void PhysEU07::sync(IController::SyncDirection direction)
{
	if (this->i2c == nullptr)
		return;

	this->conf->accessLock();
	if (direction == IController::SyncDirection::FROM_CONTROLLER) {
		I2CToConfig();
	} else {
		configToI2C();
	}
	this->conf->accessUnlock();
}

void PhysEU07::read_0x40(void)
{
	if (i2c->setSlaveAddr(0x40)) {
		log_error("Could not set I2C slave address 0x40.\n");
	} else {
		uint8_t buffer[7];

		if (i2c->readData(buffer, sizeof(buffer)) == sizeof(buffer)) {
			uint32_t main_break = (((buffer[3] & 0xff) << 8) | (buffer[4] & 0xff));
			uint32_t loc_break = (((buffer[5] & 0xff) << 8) | (buffer[6] & 0xff));

			uint32_t prev_main_break = conf->getValue(CONFIGURATION_ID_MAIN_BREAK_VALUE);
			uint32_t prev_loc_break = conf->getValue(CONFIGURATION_ID_LOC_BREAK_VALUE);

			conf->setValue(CONFIGURATION_ID_CONTROLLER_TRAIN_DIRECTION, (buffer[0] & 0xff));
			conf->setValue(CONFIGURATION_ID_CONTROLLER_ADJUSTER_WHEEL_POSITION, (buffer[1] & 0xff));
			conf->setValue(CONFIGURATION_ID_CONTROLLER_SHUNT_POSITION, (buffer[2] & 0xff));

			if (DIFF(main_break, prev_main_break) >= 10)
				conf->setValue(CONFIGURATION_ID_MAIN_BREAK_VALUE, (((buffer[3] & 0xff) << 8) | (buffer[4] & 0xff)));

			if (DIFF(loc_break, prev_loc_break) >= 10)
				conf->setValue(CONFIGURATION_ID_LOC_BREAK_VALUE, (((buffer[5] & 0xff) << 8) | (buffer[6] & 0xff)));
		}
	}
}

void PhysEU07::I2CToConfig(void)
{
	read_0x40();
}

void PhysEU07::configToI2C(void)
{

}
