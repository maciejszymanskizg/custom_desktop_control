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
		Configuration *external_configuration;

	protected:
		bool addConfigurationEntry(const uint32_t id, ConfigurationEntry *entry);
	public:
		const uint32_t CONFIGURATION_INVALID_VALUE = (uint32_t) -1;

		enum UpdateSource
		{
			CONFIGURATION_UPDATE_SOURCE_EXTERNAL,
			CONFIGURATION_UPDATE_SOURCE_INTERNAL
		};

		Configuration(const char *name, uint32_t size);
		Configuration(Configuration & other, const char *name);
		~Configuration();

		Configuration *clone(const char *name);

		void addExternalConfiguration(Configuration *conf);

		void accessLock(void);
		void accessUnlock(void);

		const char *getName(void);
		ConfigurationEntry *getEntry(const uint32_t id);

		bool getValue(const uint32_t id, uint32_t & value);
		uint32_t getValue(const uint32_t id);
		uint32_t getMaxValue(const uint32_t id);

		bool setValue(const uint32_t id, uint32_t value);
		bool setValue(const uint32_t id, uint32_t value, enum UpdateSource source);

		bool isUpdated(const uint32_t id);
		bool checkUpdates(const uint32_t *ids, uint32_t elems);
		bool checkUpdates(const uint32_t flags);
		void cleanUpdate(const uint32_t id);
		void cleanUpdates(void);
		void cleanUpdates(const uint32_t flags);

		bool setFlags(const uint32_t id, const uint32_t flags);
		bool addFlags(const uint32_t id, const uint32_t flags);
		bool getFlags(const uint32_t id, uint32_t & flags);
		uint32_t getFlags(const uint32_t id);

		void dumpConfig(void);
		bool dumpConfigUpdates(void);
		void dumpPointers(void);
};

#endif /* CONFIGURATION_H */
