#ifndef TRAIN_CONFIGURATION_H
#define TRAIN_CONFIGURATION_H

#include "Configuration.h"

struct Train_Configuration_Entry {
	unsigned int id;
	const char *name;
	unsigned int min_value;
	unsigned int max_value;
	unsigned int init_value;
};

class TrainConfiguration : public Configuration {
	public:
		TrainConfiguration();
};

#endif /* TRAIN_CONFIGURATION_H */
