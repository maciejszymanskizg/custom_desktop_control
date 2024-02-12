#ifndef CONFIGURATION_ENTRY_H
#define CONFIGURATION_ENTRY_H

#include <string>

class ConfigurationEntry {
	private:
		std::string name;
		unsigned int curr_value;
		unsigned int prev_value;
		unsigned int min_value;
		unsigned int max_value;

	public:
		ConfigurationEntry(const std::string & name, unsigned int min_value, unsigned int max_value, unsigned int init_value);
		std::string & getName(void);
		unsigned int getValue(void);
		unsigned int getPrevValue(void);
		bool setValue(unsigned int value);
		bool isUpdated(void);
		void cleanUpdate(void);
};

#endif /* CONFIGURATION_ENTRY_H */
