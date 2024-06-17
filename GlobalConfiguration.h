#ifndef GLOBAL_CONFIGURATION_H
#define GLOBAL_CONFIGURATION_H

#include "Configuration.h"

struct GlobalConfigurationEntry {
	unsigned int id;
	const char *name;
};

class GlobalConfiguration : public Configuration {
	public:
		GlobalConfiguration();
};

#endif /* GLOBAL_CONFIGURATION_H */
