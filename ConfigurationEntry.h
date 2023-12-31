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
		unsigned int getId(void);
		std::string & getName(void);
		unsigned int getValue(void);
		unsigned int getPrevValue(void);
		bool setValue(unsigned int value);
};

#endif /* CONFIGURATION_ENTRY_H */
