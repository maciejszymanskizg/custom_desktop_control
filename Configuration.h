#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdint.h>
#include "ConfigurationEntry.h"

class Configuration {
	private:
		const char *name;
		ConfigurationEntry **entries;
		uint32_t entries_count;

	protected:
		bool addConfigurationEntry(const uint32_t id, ConfigurationEntry *entry);
	public:
		const uint32_t CONFIGURATION_INVALID_VALUE = (uint32_t) -1;

		Configuration(const char *name, uint32_t size);
		~Configuration();

		const char *getName(void);
		ConfigurationEntry *getEntry(const uint32_t id);
		bool setValue(const uint32_t id, uint32_t value);
		bool getValue(const uint32_t id, uint32_t & value);
		uint32_t getValue(const uint32_t id);
		bool isUpdated(const uint32_t id);

		void dumpConfig(void);
		bool dumpConfigUpdates(void);

		void cleanUpdates(const uint32_t id);
		void cleanUpdates(void);

		bool checkUpdates(const uint32_t *ids, uint32_t elems);

		void dumpPointers(void);
};

#endif /* CONFIGURATION_H */
