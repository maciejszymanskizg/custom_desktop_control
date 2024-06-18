#include "GlobalConfiguration.h"
#include "GlobalConfigurationIDs.h"
#include "Logger.h"

#define GLOBAL_CONFIGURATION_ENTRY(_id, _name, _min, _max, _init) \
	{ \
		.id = _id, \
		.name = _name, \
		.min = _min, \
		.max = _max, \
		.init = _init \
	}

#define GLOBAL_CONFIGURATION_ENTRY_BOOL(_id, _name, _init) GLOBAL_CONFIGURATION_ENTRY(_id, _name, 0, 1, _init)

struct GlobalConfigurationEntry Global_Configuration[] = {
	GLOBAL_CONFIGURATION_ENTRY(CONFIGURATION_ID_MAIN_THREAD_SLEEP_MS, "Main thread sleep ms", 0, 1000, 50),
	GLOBAL_CONFIGURATION_ENTRY_BOOL(CONFIGURATION_ID_AUTO_BLINK_ALERTER, "Auto blink alerter", 0),
	GLOBAL_CONFIGURATION_ENTRY(CONFIGURATION_ID_AUTO_BLINK_ALERTER_MS, "Auto blink alerter ms", 0, 1000, 500),
};

GlobalConfiguration::GlobalConfiguration() : Configuration("Global Configuration")
{
	for (unsigned int i = 0; i < sizeof(Global_Configuration) / sizeof(struct GlobalConfigurationEntry); i++) {
		ConfigurationEntry *e = new ConfigurationEntry(Global_Configuration[i].name, 0, 1, 0);
		if (e != nullptr) {
			addConfigurationEntry(Global_Configuration[i].id, e);
		} else {
			log_error("Could not create configuration entry for [%s].\n", Global_Configuration[i]);
		}
	}
}
