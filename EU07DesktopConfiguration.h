#ifndef EU07_DESKTOP_CONFIGURATION_H
#define EU07_DESKTOP_CONFIGURATION_H

#include "Configuration.h"

struct EU07_Configuration_Entry {
	unsigned int id;
	const char *name;
	unsigned int min_value;
	unsigned int max_value;
	unsigned int init_value;
};

class EU07_Desktop_Configuration: private Configuration {
	public:
		EU07_Desktop_Configuration();
};

#endif /* EU07_DESKTOP_CONFIGURATION_H */
