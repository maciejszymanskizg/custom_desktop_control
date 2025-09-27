#ifndef TRAIN_CONFIGURATION_JSON_H
#define TRAIN_CONFIGURATION_JSON_H

class TrainConfigurationJson {
	public:
		static unsigned int JsonKeyToId(const char *key);
		static const char *IdToJsonKey(unsigned int id);
};

#endif /* TRAIN_CONFIGURATION_JSON_H */
