#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <map>
#include "ConfigurationEntry.h"

class Configuration {
	private:
		std::string name;
		std::map<const unsigned int, ConfigurationEntry *> entries;
	protected:
		bool addConfigurationEntry(const unsigned int id, ConfigurationEntry *entry);
	public:
		Configuration(const std::string & name);
		~Configuration();

		std::string & getName(void);
		ConfigurationEntry *getEntry(const unsigned int id);
		bool setValue(const unsigned int id, unsigned int value);
		bool getValue(const unsigned int id, unsigned int & value);
};

#endif /* CONFIGURATION_H */
