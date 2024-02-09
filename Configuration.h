#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdint.h>
#include <string>
#include <map>
#include "ConfigurationEntry.h"
#include "Mutex.h"

class Configuration {
	private:
		std::string name;
		std::map<const uint32_t, ConfigurationEntry *> entries;
		Mutex mutex;

	protected:
		bool addConfigurationEntry(const uint32_t id, ConfigurationEntry *entry);
	public:
		Configuration(const std::string & name);
		~Configuration();

		std::string & getName(void);
		ConfigurationEntry *getEntry(const uint32_t id);
		bool setValue(const uint32_t id, uint32_t value);
		bool getValue(const uint32_t id, uint32_t & value);

		void accessLock(void);
		void accessUnlock(void);

		void dumpConfig(void);
		bool dumpConfigUpdates(void);
		void cleanUpdates(void);

		bool checkUpdates(uint32_t *ids, size_t elems);
};

#endif /* CONFIGURATION_H */
