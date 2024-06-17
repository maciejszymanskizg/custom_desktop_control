#include "GlobalConfiguration.h"
#include "GlobalConfigurationIDs.h"
#include "Logger.h"

#define GLOBAL_CONFIGURATION_ENTRY(_id, _name) \
	{ \
		.id = _id, \
		.name = _name \
	}

struct GlobalConfigurationEntry Global_Configuration[] = {
	GLOBAL_CONFIGURATION_ENTRY(CONFIGURATION_ID_AUTO_BLINK_SHP, "Auto blink SHP"),
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
