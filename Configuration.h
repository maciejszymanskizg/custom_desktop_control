#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdint.h>
#include <string>
#include <map>
#include "ConfigurationEntry.h"
#include "Mutex.h"

class Configuration {
	private:
		char *name;
		std::map<const uint32_t, ConfigurationEntry *> *entries;
		Mutex *mutex;

	protected:
		bool addConfigurationEntry(const uint32_t id, ConfigurationEntry *entry);
	public:
		const uint32_t CONFIGURATION_INVALID_VALUE = (uint32_t) -1;

		Configuration(const char *name);
		~Configuration();

		const char *getName(void);
		ConfigurationEntry *getEntry(const uint32_t id);
		bool setValue(const uint32_t id, uint32_t value);
		bool getValue(const uint32_t id, uint32_t & value);
		uint32_t getValue(const uint32_t id);
		bool isUpdated(const uint32_t id);

		void accessLock(void);
		void accessUnlock(void);

		void dumpConfig(void);
		bool dumpConfigUpdates(void);

		void cleanUpdates(const uint32_t id);
		void cleanUpdates(void);

		bool checkUpdates(const uint32_t *ids, size_t elems);
};

#endif /* CONFIGURATION_H */
