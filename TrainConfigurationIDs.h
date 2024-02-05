#ifndef TRAIN_CONFIGURATION_IDS_H
#define TRAIN_CONFIGURATION_IDS_H

enum Configuration_IDs {
	/* data read from simulation software */
	CONFIGURATION_ID_HASLER_VELOCITY = 0U,
	CONFIGURATION_ID_ODOMETER,
	CONFIGURATION_ID_VOLTMETER_LOW_VOLTAGE,
	CONFIGURATION_ID_VOLTMETER_HIGH_VOLTAGE,
	CONFIGURATION_ID_AMMETER_LOW_VOLTAGE,
	CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE1,
	CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE2,
	CONFIGURATION_ID_AMMETER_HIGH_VOLTAGE3,
	CONFIGURATION_ID_TANK_PRESSURE,
	CONFIGURATION_ID_BREAK_PRESSURE,
	CONFIGURATION_ID_PIPE_PRESSURE,
	CONFIGURATION_ID_PANTOGRAH_PRESSURE,
	CONFIGURATION_ID_INDICATOR_SHP,
	CONFIGURATION_ID_INDICATOR_ALERTER,
	CONFIGURATION_ID_INDICATOR_BUZZER,
	CONFIGURATION_ID_INDICATOR_COMPRESSOR_OVERLOAD,
	CONFIGURATION_ID_INDICATOR_VENTILATOR_OVERLOAD,
	CONFIGURATION_ID_INDICATOR_LINE_BREAKER,
	CONFIGURATION_ID_INDICATOR_TRACTION_ENGINE_OVERLOAD,
	CONFIGURATION_ID_INDICATOR_MAIN_CIRCUIT_DIFFERENTIAL,
	CONFIGURATION_ID_INDICATOR_CONVERTER_OVERLOAD,
	CONFIGURATION_ID_INDICATOR_LINE_CONTACTORS,
	CONFIGURATION_ID_INDICATOR_WHEELSLIP,
	CONFIGURATION_ID_INDICATOR_HIGH_START,
	CONFIGURATION_ID_INDICATOR_RESTISTOR_RIDE,
	CONFIGURATION_ID_INDICATOR_TRAIN_HEATING,

	/* data read from external controller */
	CONFIGURATION_ID_BUTTON_RESERVE,
	CONFIGURATION_ID_BUTTON_COMPRESSOR_OVERLOAD_UNLOCK,
	CONFIGURATION_ID_BUTTON_LINE_BREAKER_DISABLE,
	CONFIGURATION_ID_BUTTON_LINE_BREAKER_ENABLE,
	CONFIGURATION_ID_BUTTON_TRACTION_ENGINE_OVERLOAD_UNLOCK,
	CONFIGURATION_ID_BUTTON_CONVERTER_OVERLOAD_UNLOCK,
	CONFIGURATION_ID_BUTTON_LINE_CONTACTORS_DISABLE,
	CONFIGURATION_ID_BUTTON_WHEELSLIP_COUNTER_ACTION,
	CONFIGURATION_ID_BUTTON_RELAXER,
	CONFIGURATION_ID_BUTTON_SHP_ALERTER_RESET,
	CONFIGURATION_ID_BUTTON_SIREN_LOW,
	CONFIGURATION_ID_BUTTON_SIREN_HIGH,
	CONFIGURATION_ID_SWITCH_BATTERY,
	CONFIGURATION_ID_SWITCH_CABIN_ACTIVATION,
	CONFIGURATION_ID_SWITCH_LEGS_HEATING,
	CONFIGURATION_ID_SWITCH_CABIN_LIGHT_DIMM,
	CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT_DIMM,
	CONFIGURATION_ID_SWITCH_SIGNAL_LAMP1_DIMM,
	CONFIGURATION_ID_SWITCH_SIGNAL_LAMP2_DIMM,
	CONFIGURATION_ID_SWITCH_SIGNAL_LAMP_RED_LEFT,
	CONFIGURATION_ID_SWITCH_RESERVE,
	CONFIGURATION_ID_SWITCH_SIGNAL_LAMP_RED_RIGHT,
	CONFIGURATION_ID_SWITCH_MAIN_LIGHT,
	CONFIGURATION_ID_SWITCH_CABIN_LIGHT,
	CONFIGURATION_ID_SWITCH_MEASURE_INSTRUMENT_LIGHT,
	CONFIGURATION_ID_SWITCH_HIGH_VOLTAGE_BOX_LIGHT,
	CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT_LEFT,
	CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT_TOP,
	CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT_RIGHT,
	CONFIGURATION_ID_SWITCH_SHP_INDICATOR_DIMM,
	CONFIGURATION_ID_SWITCH_PANTHOGRAPH_A,
	CONFIGURATION_ID_SWITCH_COMPRESSOR,
	CONFIGURATION_ID_SWITCH_CONVERTER,
	CONFIGURATION_ID_SWITCH_ALERTER_INDICATOR_DIMM,
	CONFIGURATION_ID_SWITCH_PANTHOGRAPH_B,
	CONFIGURATION_ID_SWITCH_TRAIN_HEATING,
	CONFIGURATION_ID_SWITCH_BREAK_MODE,
	CONFIGURATION_ID_SWITCH_WHEEL_PUSH_MODE,
	CONFIGURATION_ID_SWITCH_VOLTAGE_RANGE_MODE,
	CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT1_DIMM,
	CONFIGURATION_ID_SWITCH_SIGNAL_LIGHT2_DIMM,
	CONFIGURATION_ID_SWITCH_WIPERS_MODE,
	CONFIGURATION_ID_SWITCH_RADIO_ENABLE,
	CONFIGURATION_ID_SWITCH_RADIO_CHANNEL,
	CONFIGURATION_ID_CONTROLLER_TRAIN_DIRECTION,
	CONFIGURATION_ID_CONTROLLER_ADJUSTER_WHEEL_POSITION,
	CONFIGURATION_ID_CONTROLLER_SHUNT_POSITION,
	CONFIGURATION_ID_MAIN_BREAK_VALUE,
	CONFIGURATION_ID_LOC_BREAK_VALUE,
};

#endif /* TRAIN_CONFIGURATION_IDS_H */

