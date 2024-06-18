#ifndef GLOBAL_CONFIGURATION_H
#define GLOBAL_CONFIGURATION_H

#include "Configuration.h"

struct GlobalConfigurationEntry {
	unsigned int id;
	const char *name;
	unsigned int min;
	unsigned int max;
	unsigned int init;
};

class GlobalConfiguration : public Configuration {
	public:
		GlobalConfiguration();
};

#endif /* GLOBAL_CONFIGURATION_H */
