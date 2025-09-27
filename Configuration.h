#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdint.h>
#include "ConfigurationEntry.h"
#include "Mutex.h"

class Configuration {
	private:
		const char *name;
		ConfigurationEntry **entries;
		uint32_t entries_count;
		Mutex *mutex;

	protected:
		bool addConfigurationEntry(const uint32_t id, ConfigurationEntry *entry);
	public:
		const uint32_t CONFIGURATION_INVALID_VALUE = (uint32_t) -1;

		Configuration(const char *name, uint32_t size);
		~Configuration();

		void accessLock(void);
		void accessUnlock(void);

		const char *getName(void);
		ConfigurationEntry *getEntry(const uint32_t id);
		bool setValue(const uint32_t id, uint32_t value);
		bool getValue(const uint32_t id, uint32_t & value);
		uint32_t getValue(const uint32_t id);
		uint32_t getMaxValue(const uint32_t id);
		bool isUpdated(const uint32_t id);
		bool setGroupId(const uint32_t id, const uint32_t group_id);
		bool getGroupId(const uint32_t id, uint32_t & value);

		void dumpConfig(void);
		bool dumpConfigUpdates(void);

		void cleanUpdates(const uint32_t id);
		void cleanUpdates(void);
		void cleanGroupUpdates(const uint32_t group_id);

		bool checkUpdates(const uint32_t *ids, uint32_t elems);
		bool checkGroupUpdates(uint32_t group_id);

		void dumpPointers(void);
};

#endif /* CONFIGURATION_H */
