#include "PhysEU07.h"
#include "TrainConfigurationIDs.h"
#include "GlobalConfigurationIDs.h"
#include "Logger.h"

#define DIFF(x, y) (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))
#define BREAK_ADC_THRESHOLD 5
#define AMMETER_THRESHOLD 5
#define VOLTMETER_THRESHOLD 2
#define FIXED_DATA_BUFFER_SIZE_2_VALUES 4
#define FIXED_DATA_BUFFER_SIZE_3_VALUES 6

#define BUFFER_SIZE_READ_0x40 7
#define BUFFER_SIZE_READ_0x41 4

#define BUFFER_SIZE_WRITE_0x41 2
#define BUFFER_SIZE_WRITE_0x42 FIXED_DATA_BUFFER_SIZE_2_VALUES
#define BUFFER_SIZE_WRITE_0x43 FIXED_DATA_BUFFER_SIZE_2_VALUES
#define BUFFER_SIZE_WRITE_0x44 FIXED_DATA_BUFFER_SIZE_3_VALUES
#define BUFFER_SIZE_WRITE_0x45 FIXED_DATA_BUFFER_SIZE_2_VALUES

#define PIN_TO_CONFIG(_pcf8575, _pin, _conf, _config_id) \
	_conf->setValue(_config_id, !(_pcf8575->getInput(PCF8575::PIN_P##_pin)))

#define CONFIG_TO_PIN(_conf, _config_id, _pcf8575, _pin) \
	_pcf8575->setOutput(PCF8575::PIN_P##_pin, (_conf->getValue(_config_id) ? PCF8575::PinStateLow : PCF8575::PinStateHigh))

PhysEU07::PhysEU07(Configuration *train_conf, Configuration *global_conf, ICommunicationHandler *handler) : IController(ControllerType::PHYS_CONTROLLER)
{
	this->conf = train_conf;
	this->global_conf = global_conf;
	this->i2c = dynamic_cast<I2C *>(handler);
	if (this->i2c == nullptr) {
		log_error("Invalid communication handler.\n");
		pcf8575_0x20 = nullptr;
		pcf8575_0x21 = nullptr;
		pcf8575_0x22 = nullptr;
		pcf8575_0x23 = nullptr;
		pcf8575_0x24 = nullptr;
	} else {
		pcf8575_0x20 = new PCF8575(handler, 0, 0, 0);
		pcf8575_0x21 = new PCF8575(handler, 1, 0, 0);
		pcf8575_0x22 = new PCF8575(handler, 0, 1, 0);
		pcf8575_0x23 = new PCF8575(handler, 1, 1, 0);
		pcf8575_0x24 = new PCF8575(handler, 0, 0, 1);

		pcf8575_0x20->setup(0xffff);
		pcf8575_0x21->setup(0xffff);
		pcf8575_0x22->setup(0xffff);
		pcf8575_0x23->setup(0xffff);
		pcf8575_0x24->setup(0xffff);
	}

	setupConfigurationGroups();
}

PhysEU07::~PhysEU07()
{
	if (pcf8575_0x20 != nullptr)
		delete pcf8575_0x20;

	if (pcf8575_0x21 != nullptr)
		delete pcf8575_0x21;

	if (pcf8575_0x22 != nullptr)
		delete pcf8575_0x22;

	if (pcf8575_0x23 != nullptr)
		delete pcf8575_0x23;

	if (pcf8575_0x24 != nullptr)
		delete pcf8575_0x24;
}

void PhysEU07::setupConfigurationGroups(void)
{
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_BUZZER, 0x20);
	conf->setGroupId(CONFIGURATION_ID_SWITCH_SHP_INDICATOR_DIMM, 0x20);
	conf->setGroupId(CONFIGURATION_ID_SWITCH_ALERTER_INDICATOR_DIMM, 0x20);
	conf->setGroupId(CONFIGURATION_ID_AUTO_BLINK_ALERTER, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_ALERTER, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_SHP, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_RESTISTOR_RIDE, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_TRAIN_HEATING, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_HIGH_START, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_WHEELSLIP, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_LINE_CONTACTORS, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_VENTILATOR_OVERLOAD, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_COMPRESSOR_OVERLOAD, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_CONVERTER_OVERLOAD, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_MAIN_CIRCUIT_DIFFERENTIAL, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_LINE_BREAKER, 0x20);
	conf->setGroupId(CONFIGURATION_ID_INDICATOR_TRACTION_ENGINE_OVERLOAD, 0x20);

	conf->setGroupId(CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT_DIMM, 0x23);
	conf->setGroupId(CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT, 0x23);
	conf->setGroupId(CONFIGURATION_ID_SWITCH_CABIN_LIGHT, 0x23);

	conf->setGroupId(CONFIGURATION_ID_SWITCH_RADIO_ENABLE, 0x41);
	conf->setGroupId(CONFIGURATION_ID_SWITCH_RADIO_MICROPHONE_ENABLE, 0x41);
	conf->setGroupId(CONFIGURATION_ID_SWITCH_RADIOSTOP, 0x41);
	conf->setGroupId(CONFIGURATION_ID_BUTTON_RADIO_EXT1, 0x41);
	conf->setGroupId(CONFIGURATION_ID_BUTTON_RADIO_EXT2, 0x41);
	conf->setGroupId(CONFIGURATION_ID_BUTTON_RADIO_EXT3, 0x41);

	conf->setGroupId(CONFIGURATION_ID_BREAK_PRESSURE, 0x42);
	conf->setGroupId(CONFIGURATION_ID_HASLER_VELOCITY, 0x42);

	conf->setGroupId(CONFIGURATION_ID_PIPE_PRESSURE, 0x43);
	conf->setGroupId(CONFIGURATION_ID_TANK_PRESSURE, 0x43);

	conf->setGroupId(CONFIGURATION_ID_AMMETER_LOW_VOLTAGE, 0x44);
	conf->setGroupId(CONFIGURATION_ID_VOLTMETER_LOW_VOLTAGE, 0x44);
	conf->setGroupId(CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE1, 0x44);

	conf->setGroupId(CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE2, 0x45);
	conf->setGroupId(CONFIGURATION_ID_VOLTMETER_HIGH_VOLTAGE, 0x45);
}

void PhysEU07::sync(IController::SyncDirection direction)
{
	if (this->i2c == nullptr)
		return;

	if (direction == IController::SyncDirection::FROM_CONTROLLER) {
		I2CToConfig();
	} else {
		configToI2C();
	}
}


void PhysEU07::read_0x21(void)
{
	if (pcf8575_0x21 == nullptr)
		return;

	pcf8575_0x21->sync(PCF8575::SYNC_FROM_PCF8575);

	conf->accessLock();

	PIN_TO_CONFIG(pcf8575_0x21, 0, conf, CONFIGURATION_ID_BUTTON_WHEELSLIP_COUNTER_ACTION);
	PIN_TO_CONFIG(pcf8575_0x21, 1, conf, CONFIGURATION_ID_SWITCH_ALERTER_INDICATOR_DIMM);
	PIN_TO_CONFIG(pcf8575_0x21, 2, conf, CONFIGURATION_ID_SWITCH_PANTHOGRAPH_B);
	PIN_TO_CONFIG(pcf8575_0x21, 3, conf, CONFIGURATION_ID_SWITCH_CONVERTER);
	PIN_TO_CONFIG(pcf8575_0x21, 4, conf, CONFIGURATION_ID_SWITCH_SHP_INDICATOR_DIMM);
	PIN_TO_CONFIG(pcf8575_0x21, 5, conf, CONFIGURATION_ID_SWITCH_PANTHOGRAPH_A);
	PIN_TO_CONFIG(pcf8575_0x21, 6, conf, CONFIGURATION_ID_SWITCH_COMPRESSOR);
	PIN_TO_CONFIG(pcf8575_0x21, 7, conf, CONFIGURATION_ID_SWITCH_TRAIN_HEATING);

	PIN_TO_CONFIG(pcf8575_0x21, 10, conf, CONFIGURATION_ID_BUTTON_LINE_CONTACTORS_DISABLE);
	PIN_TO_CONFIG(pcf8575_0x21, 11, conf, CONFIGURATION_ID_BUTTON_COMPRESSOR_OVERLOAD_UNLOCK);
	PIN_TO_CONFIG(pcf8575_0x21, 12, conf, CONFIGURATION_ID_BUTTON_TRACTION_ENGINE_OVERLOAD_UNLOCK);
	PIN_TO_CONFIG(pcf8575_0x21, 13, conf, CONFIGURATION_ID_BUTTON_CONVERTER_OVERLOAD_UNLOCK);
	PIN_TO_CONFIG(pcf8575_0x21, 14, conf, CONFIGURATION_ID_BUTTON_RELAXER);
	PIN_TO_CONFIG(pcf8575_0x21, 15, conf, CONFIGURATION_ID_BUTTON_RESERVE);
	PIN_TO_CONFIG(pcf8575_0x21, 16, conf, CONFIGURATION_ID_BUTTON_LINE_BREAKER_ENABLE);
	PIN_TO_CONFIG(pcf8575_0x21, 17, conf, CONFIGURATION_ID_BUTTON_LINE_BREAKER_DISABLE);

	conf->accessUnlock();
}

void PhysEU07::read_0x22(void)
{
	unsigned int value = 0;

	if (pcf8575_0x22 == nullptr)
		return;

	pcf8575_0x22->sync(PCF8575::SYNC_FROM_PCF8575);

	conf->accessLock();

	PIN_TO_CONFIG(pcf8575_0x22, 0, conf, CONFIGURATION_ID_BUTTON_SIREN_LOW);
	PIN_TO_CONFIG(pcf8575_0x22, 1, conf, CONFIGURATION_ID_BUTTON_SIREN_HIGH);

	PIN_TO_CONFIG(pcf8575_0x22, 6, conf, CONFIGURATION_ID_SWITCH_BATTERY);
	PIN_TO_CONFIG(pcf8575_0x22, 7, conf, CONFIGURATION_ID_BUTTON_SHP_ALERTER_RESET);

	PIN_TO_CONFIG(pcf8575_0x22, 11, conf, CONFIGURATION_ID_SWITCH_SIGNAL_LAMP1_DIMM);
	PIN_TO_CONFIG(pcf8575_0x22, 12, conf, CONFIGURATION_ID_SWITCH_WHEEL_PUSH_MODE);
	PIN_TO_CONFIG(pcf8575_0x22, 14, conf, CONFIGURATION_ID_SWITCH_SIGNAL_LAMP2_DIMM);
	PIN_TO_CONFIG(pcf8575_0x22, 15, conf, CONFIGURATION_ID_SWITCH_VOLTAGE_RANGE_MODE);

	value = (pcf8575_0x22->getInput(PCF8575::PIN_P2) == PCF8575::PinStateLow) +
			(pcf8575_0x22->getInput(PCF8575::PIN_P3) == PCF8575::PinStateLow) * 2 +
			(pcf8575_0x22->getInput(PCF8575::PIN_P4) == PCF8575::PinStateLow) * 3;

	conf->setValue(CONFIGURATION_ID_SWITCH_WIPERS_MODE, value);

	value = (pcf8575_0x22->getInput(PCF8575::PIN_P10) == PCF8575::PinStateLow) ? 0 :
		(pcf8575_0x22->getInput(PCF8575::PIN_P13) == PCF8575::PinStateLow) ? 2 : 1;

	conf->setValue(CONFIGURATION_ID_SWITCH_BREAK_MODE, value);

	conf->accessUnlock();
}

void PhysEU07::read_0x24(void)
{
	if (pcf8575_0x24 == nullptr)
		return;

	pcf8575_0x24->sync(PCF8575::SYNC_FROM_PCF8575);

	conf->accessLock();

	PIN_TO_CONFIG(pcf8575_0x24, 0, conf, CONFIGURATION_ID_SWITCH_MAIN_LIGHT);
	PIN_TO_CONFIG(pcf8575_0x24, 1, conf, CONFIGURATION_ID_SWITCH_CABIN_LIGHT);
	PIN_TO_CONFIG(pcf8575_0x24, 2, conf, CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT);
	PIN_TO_CONFIG(pcf8575_0x24, 3, conf, CONFIGURATION_ID_SWITCH_HIGH_VOLTAGE_BOX_LIGHT);
	PIN_TO_CONFIG(pcf8575_0x24, 4, conf, CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT_LEFT);
	PIN_TO_CONFIG(pcf8575_0x24, 5, conf, CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT_TOP);
	PIN_TO_CONFIG(pcf8575_0x24, 6, conf, CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT_RIGHT);
	PIN_TO_CONFIG(pcf8575_0x24, 7, conf, CONFIGURATION_ID_SWITCH_CABIN_ACTIVATION);

	PIN_TO_CONFIG(pcf8575_0x24, 10, conf, CONFIGURATION_ID_SWITCH_LEGS_HEATING);
	PIN_TO_CONFIG(pcf8575_0x24, 11, conf, CONFIGURATION_ID_SWITCH_CABIN_LIGHT_DIMM);
	PIN_TO_CONFIG(pcf8575_0x24, 12, conf, CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT_DIMM);
	PIN_TO_CONFIG(pcf8575_0x24, 13, conf, CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT1_DIMM);
	PIN_TO_CONFIG(pcf8575_0x24, 14, conf, CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT2_DIMM);
	PIN_TO_CONFIG(pcf8575_0x24, 15, conf, CONFIGURATION_ID_SWITCH_SIGNAL_LAMP_RED_LEFT);
	PIN_TO_CONFIG(pcf8575_0x24, 16, conf, CONFIGURATION_ID_SWITCH_RESERVE);
	PIN_TO_CONFIG(pcf8575_0x24, 17, conf, CONFIGURATION_ID_SWITCH_SIGNAL_LAMP_RED_RIGHT);

	conf->accessUnlock();
}

void PhysEU07::read_0x40(void)
{
	if (i2c->setSlaveAddr(0x40)) {
		log_error("Could not set I2C slave address 0x40.\n");
	} else {
		uint8_t buffer[BUFFER_SIZE_READ_0x40];

		if (i2c->readData(buffer, sizeof(buffer)) == sizeof(buffer)) {
			uint32_t main_break = (((buffer[3] & 0xff) << 8) | (buffer[4] & 0xff));
			uint32_t loc_break = (((buffer[5] & 0xff) << 8) | (buffer[6] & 0xff));

			conf->accessLock();

			uint32_t prev_main_break = conf->getValue(CONFIGURATION_ID_MAIN_BREAK_VALUE);
			uint32_t prev_loc_break = conf->getValue(CONFIGURATION_ID_LOC_BREAK_VALUE);

			conf->setValue(CONFIGURATION_ID_CONTROLLER_TRAIN_DIRECTION, (buffer[0] & 0xff));
			conf->setValue(CONFIGURATION_ID_CONTROLLER_ADJUSTER_WHEEL_POSITION, (buffer[1] & 0xff));
			conf->setValue(CONFIGURATION_ID_CONTROLLER_SHUNT_POSITION, (buffer[2] & 0xff));

			if (DIFF(main_break, prev_main_break) > BREAK_ADC_THRESHOLD)
				conf->setValue(CONFIGURATION_ID_MAIN_BREAK_VALUE, (((buffer[3] & 0xff) << 8) | (buffer[4] & 0xff)));

			if (DIFF(loc_break, prev_loc_break) > BREAK_ADC_THRESHOLD)
				conf->setValue(CONFIGURATION_ID_LOC_BREAK_VALUE, (((buffer[5] & 0xff) << 8) | (buffer[6] & 0xff)));

			conf->accessUnlock();
		}
	}
}

void PhysEU07::read_0x41(void)
{
	if (i2c->setSlaveAddr(0x41)) {
		log_error("Could not set I2C slave address 0x41.\n");
	} else {
		uint8_t buffer[BUFFER_SIZE_READ_0x41];

		if (i2c->readData(buffer, sizeof(buffer)) == sizeof(buffer)) {
			uint8_t radio_buttons = (buffer[0] & 0xff);
			uint8_t radio_channel = (buffer[1] & 0xff);
			uint8_t radio_volume = (buffer[2] & 0xff);
			uint8_t radio_listen_volume = (buffer[3]);

			uint8_t radio_enable = radio_buttons & 0x1;
			uint8_t radio_noise = (radio_buttons >> 1) & 0x1;
			uint8_t radio_stop = (radio_buttons >> 2) & 0x1;
			uint8_t radio_ext1 = (radio_buttons >> 3) & 0x1;
			uint8_t radio_ext2 = (radio_buttons >> 4) & 0x1;
			uint8_t radio_ext3 = (radio_buttons >> 5) & 0x1;
			uint8_t radio_microphone = (radio_buttons >> 6) & 0x1;

			conf->accessLock();

			conf->setValue(CONFIGURATION_ID_SWITCH_RADIO_ENABLE, radio_enable);
			conf->setValue(CONFIGURATION_ID_SWITCH_RADIO_NOISE, radio_noise);
			conf->setValue(CONFIGURATION_ID_SWITCH_RADIO_MICROPHONE_ENABLE, radio_microphone);
			conf->setValue(CONFIGURATION_ID_SWITCH_RADIOSTOP, radio_stop);
			conf->setValue(CONFIGURATION_ID_BUTTON_RADIO_EXT1, radio_ext1);
			conf->setValue(CONFIGURATION_ID_BUTTON_RADIO_EXT2, radio_ext2);
			conf->setValue(CONFIGURATION_ID_BUTTON_RADIO_EXT3, radio_ext3);
			conf->setValue(CONFIGURATION_ID_SWITCH_RADIO_CHANNEL, radio_channel);
			conf->setValue(CONFIGURATION_ID_SWITCH_RADIO_VOLUME_LEVEL, radio_volume);
			conf->setValue(CONFIGURATION_ID_SWITCH_RADIO_LISTENING_LEVEL, radio_listen_volume);

			conf->accessUnlock();
		}
	}
}

void PhysEU07::prepareDataBuffer(uint8_t *buffer, uint32_t size, uint32_t value1, uint32_t value2)
{
	if (size != FIXED_DATA_BUFFER_SIZE_2_VALUES) {
		log_error("Buffer too short (%u vs. %u)\n", (unsigned int) size, (unsigned int) FIXED_DATA_BUFFER_SIZE_2_VALUES);
		return;
	}

	buffer[0] = ((value1 >> 8) & 0xff);
	buffer[1] = (value1 & 0xff);
	buffer[2] = ((value2 >> 8) & 0xff);
	buffer[3] = (value2 & 0xff);
}

void PhysEU07::prepareDataBuffer(uint8_t *buffer, uint32_t size, uint32_t value1, uint32_t value2, uint32_t value3)
{
	if (size != FIXED_DATA_BUFFER_SIZE_3_VALUES) {
		log_error("Buffer too short (%u vs. %u)\n", (unsigned int) size, (unsigned int) FIXED_DATA_BUFFER_SIZE_3_VALUES);
		return;
	}

	buffer[0] = ((value1 >> 8) & 0xff);
	buffer[1] = (value1 & 0xff);
	buffer[2] = ((value2 >> 8) & 0xff);
	buffer[3] = (value2 & 0xff);
	buffer[4] = ((value3 >> 8) & 0xff);
	buffer[5] = (value3 & 0xff);
}

void PhysEU07::write_0x20(void)
{
	static unsigned int alerter_counter = 0;
	static bool alerter_indicator = true;

	if (pcf8575_0x20 == nullptr)
		return;

	conf->accessLock();

	bool changed = conf->checkGroupUpdates(0x20);

	if (changed) {

		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_BUZZER, pcf8575_0x20, 0);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_SWITCH_SHP_INDICATOR_DIMM, pcf8575_0x20, 1);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_SWITCH_ALERTER_INDICATOR_DIMM, pcf8575_0x20, 2);

		if (global_conf->getValue(CONFIGURATION_ID_AUTO_BLINK_ALERTER)) {
			if (conf->getValue(CONFIGURATION_ID_INDICATOR_ALERTER)) {
				alerter_counter++;

				if (alerter_counter == (global_conf->getValue(CONFIGURATION_ID_AUTO_BLINK_ALERTER_MS) /
							global_conf->getValue(CONFIGURATION_ID_OUTPUT_THREAD_SLEEP_US / 1000))) {
					alerter_counter = 0;
					alerter_indicator = !alerter_indicator;
				}

				pcf8575_0x20->setOutput(PCF8575::PIN_P4, (alerter_indicator ? PCF8575::PinStateLow : PCF8575::PinStateHigh));
			} else {
				pcf8575_0x20->setOutput(PCF8575::PIN_P4, PCF8575::PinStateHigh);
			}
		} else {
			CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_ALERTER, pcf8575_0x20, 4);
		}

		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_SHP, pcf8575_0x20, 3);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_RESTISTOR_RIDE, pcf8575_0x20, 5);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_TRAIN_HEATING, pcf8575_0x20, 6);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_HIGH_START, pcf8575_0x20, 7);

		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_WHEELSLIP, pcf8575_0x20, 10);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_LINE_CONTACTORS, pcf8575_0x20, 11);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_VENTILATOR_OVERLOAD, pcf8575_0x20, 12);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_COMPRESSOR_OVERLOAD, pcf8575_0x20, 13);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_CONVERTER_OVERLOAD, pcf8575_0x20, 14);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_MAIN_CIRCUIT_DIFFERENTIAL, pcf8575_0x20, 15);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_LINE_BREAKER, pcf8575_0x20, 16);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_INDICATOR_TRACTION_ENGINE_OVERLOAD, pcf8575_0x20, 17);

		conf->cleanGroupUpdates(0x20);
	}

	conf->accessUnlock();

	if (changed)
		pcf8575_0x20->sync(PCF8575::SYNC_TO_PCF8575);
}

void PhysEU07::write_0x23(void)
{
	if (pcf8575_0x23 == nullptr)
		return;

	conf->accessLock();

	bool changed = conf->checkGroupUpdates(0x23);

	if (changed) {
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT_DIMM, pcf8575_0x23, 10);
		CONFIG_TO_PIN(conf, CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT, pcf8575_0x23, 11);

		CONFIG_TO_PIN(conf, CONFIGURATION_ID_SWITCH_CABIN_LIGHT, pcf8575_0x23, 17);

		conf->cleanGroupUpdates(0x23);
	}

	conf->accessUnlock();

	if (changed)
		pcf8575_0x23->sync(PCF8575::SYNC_TO_PCF8575);
}

void PhysEU07::write_0x41(void)
{
	if (i2c->setSlaveAddr(0x41)) {
		log_error("Cound not set I2C slave address 0x41.\n");
	} else {
		uint8_t buffer[BUFFER_SIZE_WRITE_0x41];
		static uint8_t old_radio_enable = 0;
		static uint8_t old_sfn = 0;

		uint8_t radio_enable = 0;
		uint8_t sfn = 0;

		conf->accessLock();

		bool changed = conf->checkGroupUpdates(0x41);

		if (changed) {
			radio_enable = conf->getValue(CONFIGURATION_ID_SWITCH_RADIO_ENABLE);
			sfn = conf->getValue(CONFIGURATION_ID_SWITCH_RADIO_MICROPHONE_ENABLE) |
				conf->getValue(CONFIGURATION_ID_SWITCH_RADIOSTOP) |
				conf->getValue(CONFIGURATION_ID_BUTTON_RADIO_EXT1) |
				conf->getValue(CONFIGURATION_ID_BUTTON_RADIO_EXT2) |
				conf->getValue(CONFIGURATION_ID_BUTTON_RADIO_EXT3);
			sfn &= radio_enable;

			conf->cleanGroupUpdates(0x41);
		}

		conf->accessUnlock();

		if (changed) {
			buffer[0] = radio_enable;
			buffer[1] = sfn;

			if ((radio_enable != old_radio_enable) ||
					(sfn != old_sfn)) {

				if (i2c->writeData(buffer, sizeof(buffer)) != sizeof(buffer)) {
					log_error("Error in writing data to 0x41.\n");
				}

				old_radio_enable = radio_enable;
				old_sfn = sfn;
			}
		}
	}
}

void PhysEU07::write_0x42(void)
{
	if (i2c->setSlaveAddr(0x42)) {
		log_error("Cound not set I2C slave address 0x42.\n");
	} else {
		uint8_t buffer[BUFFER_SIZE_WRITE_0x42];
		uint32_t break_pressure = 0;
		uint32_t hasler_velocity = 0;

		conf->accessLock();

		bool changed = conf->checkGroupUpdates(0x42);

		if (changed) {
			break_pressure = conf->getValue(CONFIGURATION_ID_BREAK_PRESSURE);
			hasler_velocity = conf->getValue(CONFIGURATION_ID_HASLER_VELOCITY);

			conf->cleanGroupUpdates(0x42);
		}

		conf->accessUnlock();

		if (changed) {

			prepareDataBuffer(buffer, sizeof(buffer), break_pressure, hasler_velocity);

			if (i2c->writeData(buffer, sizeof(buffer)) != sizeof(buffer)) {
				log_error("Error in writing data to 0x42.\n");
			}
		}
	}
}

void PhysEU07::write_0x43(void)
{
	if (i2c->setSlaveAddr(0x43)) {
		log_error("Cound not set I2C slave address 0x43.\n");
	} else {
		uint8_t buffer[BUFFER_SIZE_WRITE_0x43];
		uint32_t pipe_pressure = 0;
		uint32_t tank_pressure = 0;

		conf->accessLock();

		bool changed = conf->checkGroupUpdates(0x43);

		if (changed) {
			pipe_pressure = conf->getValue(CONFIGURATION_ID_PIPE_PRESSURE);
			tank_pressure = conf->getValue(CONFIGURATION_ID_TANK_PRESSURE);

			conf->cleanGroupUpdates(0x43);
		}

		conf->accessUnlock();

		if (changed) {
			prepareDataBuffer(buffer, sizeof(buffer), pipe_pressure, tank_pressure);

			if (i2c->writeData(buffer, sizeof(buffer)) != sizeof(buffer)) {
				log_error("Error in writing data to 0x42.\n");
			}
		}
	}
}

void PhysEU07::write_0x44(void)
{
	if (i2c->setSlaveAddr(0x44)) {
		log_error("Cound not set I2C slave address 0x44.\n");
	} else {
		uint8_t buffer[BUFFER_SIZE_WRITE_0x44];
		static uint32_t old_alv_value = 0;
		static uint32_t old_vlv_value = 0;
		static uint32_t old_ahv_value = 0;
		uint32_t alv_value = 0;
		uint32_t vlv_value = 0;
		uint32_t ahv_value = 0;

		conf->accessLock();

		bool changed = conf->checkGroupUpdates(0x44);

		if (changed) {
			alv_value = conf->getValue(CONFIGURATION_ID_AMMETER_LOW_VOLTAGE);
			vlv_value = conf->getValue(CONFIGURATION_ID_VOLTMETER_LOW_VOLTAGE);
			ahv_value = conf->getValue(CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE1);

			conf->cleanGroupUpdates(0x44);
		}

		conf->accessUnlock();

		if (changed) {
			if ((DIFF(old_alv_value, alv_value) >= AMMETER_THRESHOLD) ||
					(DIFF(old_vlv_value, vlv_value) >= VOLTMETER_THRESHOLD) ||
					(DIFF(old_ahv_value, ahv_value) >= AMMETER_THRESHOLD)) {

				prepareDataBuffer(buffer, sizeof(buffer), alv_value, vlv_value, ahv_value);

				if (i2c->writeData(buffer, sizeof(buffer)) != sizeof(buffer)) {
					log_error("Error in writing data to 0x43.\n");
				}

				old_alv_value = alv_value;
				old_vlv_value = vlv_value;
				old_ahv_value = ahv_value;
			}
		}
	}
}

void PhysEU07::write_0x45(void)
{
	if (i2c->setSlaveAddr(0x45)) {
		log_error("Cound not set I2C slave address 0x45.\n");
	} else {
		uint8_t buffer[BUFFER_SIZE_WRITE_0x45];
		static uint32_t old_ahv_value = 0;
		static uint32_t old_vhv_value = 0;
		uint32_t ahv_value = 0;
		uint32_t vhv_value = 0;

		conf->accessLock();

		bool changed = conf->checkGroupUpdates(0x45);

		if (changed) {
			ahv_value = conf->getValue(CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE2);
			vhv_value = conf->getValue(CONFIGURATION_ID_VOLTMETER_HIGH_VOLTAGE);

			conf->cleanGroupUpdates(0x45);
		}

		conf->accessUnlock();

		if (changed) {
			if ((DIFF(old_ahv_value, ahv_value) >= AMMETER_THRESHOLD) ||
					(DIFF(old_vhv_value, vhv_value) >= VOLTMETER_THRESHOLD)) {

				prepareDataBuffer(buffer, sizeof(buffer), ahv_value, vhv_value);

				if (i2c->writeData(buffer, sizeof(buffer)) != sizeof(buffer)) {
					log_error("Error in writing data to 0x43.\n");
				}

				old_ahv_value = ahv_value;
				old_vhv_value = vhv_value;
			}
		}
	}
}

void PhysEU07::I2CToConfig(void)
{
	read_0x21();
	read_0x22();
	read_0x24();
	read_0x40();
	read_0x41();
}

void PhysEU07::configToI2C(void)
{
	write_0x41();
	write_0x42();
	write_0x43();
	write_0x44();
	write_0x45();
	write_0x20();
	write_0x23();
}
