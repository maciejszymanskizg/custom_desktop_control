#ifndef CONFIGURATION_ENTRY_H
#define CONFIGURATION_ENTRY_H

#include <stdint.h>

#define GROUP_ID_UNKNOWN ((uint32_t) -1)

class ConfigurationEntry {
	public:
		enum UpdateSource {
			CONFIGURATION_ENTRY_UPDATE_SOURCE_EXTERNAL,
			CONFIGURATION_ENTRY_UPDATE_SOURCE_INTERNAL
		};

	private:
		const char *name;
		uint32_t id;
		uint32_t curr_value;
		uint32_t prev_value;
		uint32_t min_value;
		uint32_t max_value;
		uint32_t flags;
		bool is_updated;

	public:
		ConfigurationEntry(uint32_t id, const char *name, uint32_t min_value, uint32_t max_value, uint32_t init_value);
		ConfigurationEntry(const ConfigurationEntry & other);
		~ConfigurationEntry();

		ConfigurationEntry *clone(void);

		const char *getName(void);
		const uint32_t getId(void);
		uint32_t getValue(void);
		uint32_t getMinValue(void);
		uint32_t getMaxValue(void);
		uint32_t getPrevValue(void);
		uint32_t getFlags(void);

		bool setValue(uint32_t value);
		bool setValue(uint32_t value, enum UpdateSource source);

		void setFlags(uint32_t flags);

		bool isUpdated(void);
		void cleanUpdate(void);
};

#endif /* CONFIGURATION_ENTRY_H */
