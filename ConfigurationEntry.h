#ifndef CONFIGURATION_ENTRY_H
#define CONFIGURATION_ENTRY_H

#include <stdint.h>

#define GROUP_ID_UNKNOWN ((uint32_t) -1)

class ConfigurationEntry {
	private:
		const char *name;
		uint32_t curr_value;
		uint32_t prev_value;
		uint32_t min_value;
		uint32_t max_value;
		bool update_flag;
		uint32_t group_id;
		bool group_update_flag;

	public:
		ConfigurationEntry(const char *name, uint32_t min_value, uint32_t max_value, uint32_t init_value);
		~ConfigurationEntry();
		const char *getName(void);
		uint32_t getValue(void);
		uint32_t getMaxValue(void);
		uint32_t getPrevValue(void);
		bool setValue(uint32_t value);
		bool isUpdated(void);
		void cleanUpdate(void);
		uint32_t getGroupId(void);
		void setGroupId(uint32_t group_id);
		bool isGroupUpdated(void);
		void cleanGroupUpdate(void);
};

#endif /* CONFIGURATION_ENTRY_H */
