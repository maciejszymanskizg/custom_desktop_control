#include "GlobalConfiguration.h"
#include "GlobalConfigurationIDs.h"
#include "ConfigurationNode.h"
#include "Logger.h"

#define GLOBAL_CONFIGURATION_ENTRY(_id, _name, _min, _max, _init) \
	ConfigurationNode(_id, _name, _min, _max, _init)

#define GLOBAL_CONFIGURATION_ENTRY_BOOL(_id, _name, _init) GLOBAL_CONFIGURATION_ENTRY(_id, _name, 0, 1, _init)

struct ConfigurationNode Global_Configuration_Nodes[] = {
	GLOBAL_CONFIGURATION_ENTRY(CONFIGURATION_ID_MAIN_THREAD_SLEEP_US, "Main thread sleep us", 0, 1000000, 10000),
	GLOBAL_CONFIGURATION_ENTRY(CONFIGURATION_ID_INPUT_THREAD_SLEEP_US, "Input thread sleep us", 0, 1000000, 5000),
	GLOBAL_CONFIGURATION_ENTRY(CONFIGURATION_ID_OUTPUT_THREAD_SLEEP_US, "Output thread sleep up", 0, 1000000, 5000),
	GLOBAL_CONFIGURATION_ENTRY_BOOL(CONFIGURATION_ID_AUTO_BLINK_ALERTER, "Auto blink alerter", 0),
	GLOBAL_CONFIGURATION_ENTRY(CONFIGURATION_ID_AUTO_BLINK_ALERTER_MS, "Auto blink alerter ms", 0, 1000, 500),
	GLOBAL_CONFIGURATION_ENTRY_BOOL(CONFIGURATION_ID_MASZYNA_UART_CHECK_PACKET_RECEIVED, "Maszyna UART check packet received", 1),
};

GlobalConfiguration::GlobalConfiguration() : Configuration("Global Configuration", sizeof(Global_Configuration_Nodes) / sizeof(struct ConfigurationNode))
{
	for (unsigned int i = 0; i < sizeof(Global_Configuration_Nodes) / sizeof(struct ConfigurationNode); i++) {
		ConfigurationNode *node = &Global_Configuration_Nodes[i];
		ConfigurationEntry *e = new ConfigurationEntry(node->getName(), node->getMin(), node->getMax(), node->getInit());
		if (e != nullptr) {
			addConfigurationEntry(node->getId(), e);
		} else {
			log_error("Could not create configuration entry for [%s].\n", node->getName());
		}
	}
}
