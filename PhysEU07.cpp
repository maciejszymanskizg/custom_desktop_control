#include "PhysEU07.h"
#include "TrainConfigurationIDs.h"
#include "Logger.h"

#define DIFF(x, y) (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))
#define BREAK_ADC_THRESHOLD 5
#define PRESSURE_THRESHOLD 5
#define VELOCITY_THRESHOLD 2
#define FIXED_DATA_BUFFER_SIZE 7

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

			if (DIFF(main_break, prev_main_break) > BREAK_ADC_THRESHOLD)
				conf->setValue(CONFIGURATION_ID_MAIN_BREAK_VALUE, (((buffer[3] & 0xff) << 8) | (buffer[4] & 0xff)));

			if (DIFF(loc_break, prev_loc_break) > BREAK_ADC_THRESHOLD)
				conf->setValue(CONFIGURATION_ID_LOC_BREAK_VALUE, (((buffer[5] & 0xff) << 8) | (buffer[6] & 0xff)));
		}
	}
}

void PhysEU07::prepareDataBuffer(uint8_t *buffer, uint32_t size, uint32_t value1, uint32_t value2)
{
	if (size != FIXED_DATA_BUFFER_SIZE) {
		log_error("Buffer too short (%u vs. %u)\n", (unsigned int) size, (unsigned int) FIXED_DATA_BUFFER_SIZE);
		return;
	}

	buffer[0] = 0xBE;
	buffer[1] = ((value1 >> 8) & 0xff);
	buffer[2] = (value1 & 0xff);
	buffer[3] = ((value2 >> 8) & 0xff);
	buffer[4] = (value2 & 0xff);
	buffer[5] = 0xA6 ^ (buffer[1] & 0xff) ^ (buffer[2] & 0xff) ^ (buffer[3] & 0xff) ^ (buffer[4] & 0xff);
	buffer[6] = 0xEB;
}

void PhysEU07::write_0x42(void)
{
	if (i2c->setSlaveAddr(0x42)) {
		log_error("Cound not set I2C slave address 0x42.\n");
	} else {
		uint8_t buffer[FIXED_DATA_BUFFER_SIZE];
		static uint32_t old_break_pressure = 0;
		static uint32_t old_hasler_velocity = 0;
		uint32_t break_pressure = conf->getValue(CONFIGURATION_ID_BREAK_PRESSURE);
		uint32_t hasler_velocity = conf->getValue(CONFIGURATION_ID_HASLER_VELOCITY);

		if ((DIFF(old_break_pressure, break_pressure) >= PRESSURE_THRESHOLD) ||
				(DIFF(old_hasler_velocity, hasler_velocity) >= VELOCITY_THRESHOLD)) {

			prepareDataBuffer(buffer, sizeof(buffer), break_pressure, hasler_velocity);

			if (i2c->writeData(buffer, sizeof(buffer)) != sizeof(buffer)) {
				log_error("Error in writing data to 0x42.\n");
			}

			old_break_pressure = break_pressure;
			old_hasler_velocity = hasler_velocity;
		}
	}
}

void PhysEU07::write_0x43(void)
{
	if (i2c->setSlaveAddr(0x43)) {
		log_error("Cound not set I2C slave address 0x43.\n");
	} else {
		uint8_t buffer[FIXED_DATA_BUFFER_SIZE];
		static uint32_t old_pipe_pressure = 0;
		static uint32_t old_tank_pressure = 0;
		uint32_t pipe_pressure = conf->getValue(CONFIGURATION_ID_PIPE_PRESSURE);
		uint32_t tank_pressure = conf->getValue(CONFIGURATION_ID_TANK_PRESSURE);

		if ((DIFF(old_pipe_pressure, pipe_pressure) >= PRESSURE_THRESHOLD) ||
				(DIFF(old_tank_pressure, tank_pressure) >= PRESSURE_THRESHOLD)) {

			prepareDataBuffer(buffer, sizeof(buffer), pipe_pressure, tank_pressure);

			if (i2c->writeData(buffer, sizeof(buffer)) != sizeof(buffer)) {
				log_error("Error in writing data to 0x42.\n");
			}

			old_pipe_pressure = pipe_pressure;
			old_tank_pressure = tank_pressure;
		}
	}
}

void PhysEU07::I2CToConfig(void)
{
	read_0x40();
}

void PhysEU07::configToI2C(void)
{
	write_0x42();
	write_0x43();
}
