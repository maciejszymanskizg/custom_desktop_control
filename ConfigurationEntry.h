#ifndef CONFIGURATION_ENTRY_H
#define CONFIGURATION_ENTRY_H

#include <stdint.h>

class ConfigurationEntry {
	private:
		const char *name;
		uint32_t curr_value;
		uint32_t prev_value;
		uint32_t min_value;
		uint32_t max_value;

	public:
		ConfigurationEntry(const char *name, uint32_t min_value, uint32_t max_value, uint32_t init_value);
		~ConfigurationEntry();
		const char *getName(void);
		uint32_t getValue(void);
		uint32_t getPrevValue(void);
		bool setValue(uint32_t value);
		bool isUpdated(void);
		void cleanUpdate(void);
};

#endif /* CONFIGURATION_ENTRY_H */
