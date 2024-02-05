#include <string.h>
#include "Logger.h"
#include "MaszynaUART.h"
#include "TrainConfigurationIDs.h"

#define MASZYNA_INPUT_BUFFER_SIZE 52
#define MASZYNA_OUTPUT_BUFFER_SIZE 20

MaszynaUART::MaszynaUART(ICommunicationHandler *uart, Configuration *conf) :
	IController(IController::ControllerType::HOST_CONTROLLER)
{
	this->uart = uart;
	this->conf = conf;
}

MaszynaUART::~MaszynaUART()
{
}

void MaszynaUART::sync(IController::SyncDirection dir)
{
	if (dir == IController::SyncDirection::FROM_CONTROLLER) {
		readUART();
	} else {
		writeUART();
	}
}

void MaszynaUART::readUART(void)
{
	uint8_t buffer[MASZYNA_INPUT_BUFFER_SIZE];
	bzero(buffer, MASZYNA_INPUT_BUFFER_SIZE);

	ssize_t bytes = uart->readData(buffer, MASZYNA_INPUT_BUFFER_SIZE);
	if (bytes != MASZYNA_INPUT_BUFFER_SIZE) {
		log_error("Received only %d from expected %d bytes !\n", bytes, MASZYNA_INPUT_BUFFER_SIZE);
		return;
	}

	/* check preamble (bytes 0 - 3) */
	if ((buffer[0] != 0xEF) ||
		(buffer[1] != 0xEF) ||
		(buffer[2] != 0xEF) ||
		(buffer[3] != 0xEF)) {
		log_error("Synchronization error (preamble).\n");
		return;
	}

	/* byte 4 - 5  tacho */
	conf->setValue(CONFIGURATION_ID_HASLER_VELOCITY, ((buffer[4] << 8) | buffer[5]));

	/* byte 6 :
	 * trainstate.epbrake_enabled << 0
	 * | trainstate.ventilator_overload << 1
	 * | trainstate.motor_overload_threshold << 2
	 * | trainstate.emergencybrake << 3
	 * | trainstate.lockpipe << 4
	 * | trainstate.dir_forward << 5
	 * | trainstate.dir_backward << 6),
	 */
	conf->setValue(CONFIGURATION_ID_INDICATOR_VENTILATOR_OVERLOAD, ((buffer[6] >> 1) & 0x1));
	conf->setValue(CONFIGURATION_ID_INDICATOR_TRACTION_ENGINE_OVERLOAD, ((buffer[6] >> 2) & 0x1));

	/* byte 7 :
	 * trainstate.coupled_hv_voltage_relays << 0
	 * | trainstate.doorleftallowed << 1
	 * | trainstate.doorleftopened << 2
	 * | trainstate.doorrightallowed << 3
	 * | trainstate.doorrightopened << 4
	 * | trainstate.doorstepallowed << 5
	 * | trainstate.battery << 6
	 * | trainstate.radiomessageindicator << 7
	 */
	conf->setValue(CONFIGURATION_ID_INDICATOR_MAIN_CIRCUIT_DIFFERENTIAL, ((buffer[7] >> 0) & 0x1));

	/* byte 8 :
	 * trainstate.train_heating << 0
	 * | trainstate.motor_resistors << 1
	 * | trainstate.wheelslip << 2
	 * | trainstate.alerter << 6
	 * | trainstate.shp << 7
	 */
	conf->setValue(CONFIGURATION_ID_INDICATOR_TRAIN_HEATING, ((buffer[8] >> 0) & 0x1));
	conf->setValue(CONFIGURATION_ID_INDICATOR_RESTISTOR_RIDE, ((buffer[8] >> 1) & 0x1));
	conf->setValue(CONFIGURATION_ID_INDICATOR_WHEELSLIP, ((buffer[8] >> 2) & 0x1));
	conf->setValue(CONFIGURATION_ID_INDICATOR_ALERTER, ((buffer[8] >> 6) & 0x1));
	conf->setValue(CONFIGURATION_ID_INDICATOR_SHP, ((buffer[8] >> 7) & 0x1));

	/* byte 9 :
	 * trainstate.motor_connectors << 0
	 * | trainstate.converter_overload << 2
	 * | trainstate.ground_relay << 3
	 * | trainstate.motor_overload << 4
	 * | trainstate.line_breaker << 5
	 * | trainstate.compressor_overload << 6
	 */
	conf->setValue(CONFIGURATION_ID_INDICATOR_LINE_CONTACTORS, ((buffer[9] >> 0) & 0x1));
	conf->setValue(CONFIGURATION_ID_INDICATOR_CONVERTER_OVERLOAD, ((buffer[9] >> 2) & 0x1));
	conf->setValue(CONFIGURATION_ID_INDICATOR_LINE_BREAKER, ((buffer[9] >> 5) & 0x1));
	conf->setValue(CONFIGURATION_ID_INDICATOR_COMPRESSOR_OVERLOAD, ((buffer[9] >> 6) & 0x1));

	/* byte 10 :
	 * m_trainstatecab << 2
	 * | trainstate.recorder_braking << 3
	 * | trainstate.recorder_power << 4
	 * | trainstate.radio_stop << 5
	 * | trainstate.springbrake_active << 6
	 * | trainstate.alerter_sound << 7
	 */
	conf->setValue(CONFIGURATION_ID_INDICATOR_BUZZER, ((buffer[10] >> 7) & 0x1));

	/* byte 11 - 12 : brake_press */
	conf->setValue(CONFIGURATION_ID_BREAK_PRESSURE, ((buffer[11] << 8) | buffer[12]));

	/* byte 13 - 14 : pipe_press */
	conf->setValue(CONFIGURATION_ID_PIPE_PRESSURE, ((buffer[13] << 8) | buffer[14]));

	/* byte 15 - 16 : tank_press */
	conf->setValue(CONFIGURATION_ID_TANK_PRESSURE, ((buffer[15] << 8) | buffer[16]));

	/* byte 17 - 18 : hv_voltage */
	conf->setValue(CONFIGURATION_ID_VOLTMETER_HIGH_VOLTAGE, ((buffer[17] << 8) | buffer[18]));

	/* byte 19 - 20 : current1 */
	conf->setValue(CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE1, ((buffer[19] << 8) | buffer[20]));

	/* byte 21 - 22 : current2 */
	conf->setValue(CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE2, ((buffer[21] << 8) | buffer[22]));

	/* byte 23 - 24 : current3 */
	conf->setValue(CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE3, ((buffer[23] << 8) | buffer[24]));

	/* byte 25 - 26 : (time.wYear - 1) * 12 + time.wMonth - 1 */

	/* byte 27 - 28 : (time.wDay - 1) * 1440 + time.wHour * 60 + time.wMinute */

	/* byte 29 - 30 : time.wSecond * 1000 + time.wMilliseconds */

	/* byte 31 - 34 : odometer */
	conf->setValue(CONFIGURATION_ID_ODOMETER,
			((buffer[31] & 0xff) << 24) |
			((buffer[32] & 0xff) << 16) |
			((buffer[33] & 0xff) << 8) |
			(buffer[34] & 0xff));

	/* byte 35 - 36 : lv_voltage */
	conf->setValue(CONFIGURATION_ID_VOLTMETER_LOW_VOLTAGE, ((buffer[35] << 8) | buffer[36]));

	/* byte 37 : trainstate.radio_channel */

	/* byte 38 - 39 : pantograph_press */
	conf->setValue(CONFIGURATION_ID_PANTOGRAH_PRESSURE, ((buffer[38] & 0xff) << 8) | (buffer[39] & 0xff));

	/* byte 40 - 51 : 0 */

}

unsigned int MaszynaUART::getConfigValue(unsigned int id)
{
	unsigned int value = 0;
	if (!conf->getValue(id, value)) {
		log_warning("Could not get value for id %u\n", id);
	}

	return value;
}

void MaszynaUART::writeUART(void)
{
	uint8_t buffer[MASZYNA_OUTPUT_BUFFER_SIZE];
	bzero(buffer, MASZYNA_OUTPUT_BUFFER_SIZE);

	/* preamble */
	buffer[0] = 0xEF;
	buffer[1] = 0xEF;
	buffer[2] = 0xEF;
	buffer[3] = 0xEF;

	/* byte 4 : switch group 0 */
	buffer[4] =
		(getConfigValue(CONFIGURATION_ID_SWITCH_CABIN_ACTIVATION) & 0x1) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_LEGS_HEATING) & 0x1) << 1) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_CABIN_LIGHT_DIMM) & 0x1) << 2) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT_DIMM) & 0x1) << 3) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_SIGNAL_LAMP1_DIMM) & 0x1) << 4) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_SIGNAL_LAMP2_DIMM) & 0x1) << 5) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_SIGNAL_LAMP_RED_LEFT) & 0x1) << 6) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_RESERVE) & 0x1) << 7);

	/* byte 5 : switch group 1 */
	buffer[5] =
		(getConfigValue(CONFIGURATION_ID_SWITCH_SIGNAL_LAMP_RED_RIGHT) & 0x1) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_MAIN_LIGHT) & 0x1) << 1) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_CABIN_LIGHT) & 0x1) << 2) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT) & 0x1) << 3) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_HIGH_VOLTAGE_BOX_LIGHT) & 0x1) << 4) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT_LEFT) & 0x1) << 5) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT_TOP) & 0x1) << 6) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT_RIGHT) & 0x1) << 7);

	/* byte 6 : switch group 2 */
	buffer[6] =
		(getConfigValue(CONFIGURATION_ID_SWITCH_BATTERY) & 0x1) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_SHP_INDICATOR_DIMM) & 0x1) << 1) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_PANTHOGRAPH_A) & 0x1) << 2) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_COMPRESSOR) & 0x1) << 3) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_CONVERTER) & 0x1) << 4) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_ALERTER_INDICATOR_DIMM) & 0x1) << 5) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_PANTHOGRAPH_B) & 0x1) << 6) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_TRAIN_HEATING) & 0x1) << 7);

	/* byte 7 : switch group 3 */
	buffer[7] =
		(getConfigValue(CONFIGURATION_ID_BUTTON_RESERVE) & 0x1) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_COMPRESSOR_OVERLOAD_UNLOCK) & 0x1) << 1) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_LINE_BREAKER_DISABLE) & 0x1) << 2) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_LINE_BREAKER_ENABLE) & 0x1) << 3) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_TRACTION_ENGINE_OVERLOAD_UNLOCK) & 0x1) << 4) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_CONVERTER_OVERLOAD_UNLOCK) & 0x1) << 5) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_LINE_CONTACTORS_DISABLE) & 0x1) << 6) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_WHEELSLIP_COUNTER_ACTION) & 0x1) << 7);

	/* byte 8 : switch group 4 */
	buffer[8] =
		(getConfigValue(CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT1_DIMM) & 0x1) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT2_DIMM) & 0x1) << 1) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_RELAXER) & 0x1) << 2) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_SHP_ALERTER_RESET) & 0x1) << 3) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_SIREN_LOW) & 0x1) << 4) |
		((getConfigValue(CONFIGURATION_ID_BUTTON_SIREN_HIGH) & 0x1) << 5) |
		((getConfigValue(CONFIGURATION_ID_CONTROLLER_TRAIN_DIRECTION) & 0x3) << 6);

	/* byte 9 : switch group 5 */
	buffer[9] =
		(getConfigValue(CONFIGURATION_ID_SWITCH_BREAK_MODE) & 0x3) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_WHEEL_PUSH_MODE) & 0x1) << 2) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_VOLTAGE_RANGE_MODE) & 0x1) << 3) |
		((getConfigValue(CONFIGURATION_ID_SWITCH_WIPERS_MODE) & 0x7) << 4);

	/* byte 10 : master controller */
	buffer[10] = getConfigValue(CONFIGURATION_ID_CONTROLLER_ADJUSTER_WHEEL_POSITION) & 0xff;

	/* byte 11 : second controller */
	buffer[11] = getConfigValue(CONFIGURATION_ID_CONTROLLER_SHUNT_POSITION) & 0xff;

	/* byte 12 - 13 : train break */
	buffer[12] = (getConfigValue(CONFIGURATION_ID_MAIN_BREAK_VALUE) >> 8) & 0xff;
	buffer[13] = getConfigValue(CONFIGURATION_ID_MAIN_BREAK_VALUE) & 0xff;

	/* byte 14 - 15 : locomotive break */
	buffer[14] = (getConfigValue(CONFIGURATION_ID_LOC_BREAK_VALUE) >> 8) & 0xff;
	buffer[15] = getConfigValue(CONFIGURATION_ID_LOC_BREAK_VALUE) & 0xff;

	/* byte 16 : radio controll */
	buffer[16] = 0xF0 /* max volume */ | (getConfigValue(CONFIGURATION_ID_SWITCH_RADIO_CHANNEL) & 0xf);

	/* bytes 17 - 19 : 0 */

	ssize_t bytes = uart->writeData(buffer, MASZYNA_OUTPUT_BUFFER_SIZE);
	if (bytes != MASZYNA_OUTPUT_BUFFER_SIZE) {
		log_error("Sent only %d from expected %d bytes !\n", bytes, MASZYNA_OUTPUT_BUFFER_SIZE);
	}
}
