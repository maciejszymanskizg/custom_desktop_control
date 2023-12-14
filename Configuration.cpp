#include <utility>
#include "Configuration.h"
#include "Logger.h"

Configuration::Configuration(const std::string & name)
{
	this->name = name;
}

Configuration::~Configuration()
{
	std::map<const unsigned int, ConfigurationEntry *>::iterator it;
	ConfigurationEntry *e;

	while (entries.size() > 0) {
		e = entries.begin()->second;
		entries.erase(entries.begin());
		delete e;
	}
}

bool Configuration::addConfigurationEntry(const unsigned int id, ConfigurationEntry *entry)
{
	if (entry == nullptr)
		return false;

	std::map<const unsigned int, ConfigurationEntry *>::iterator it =
		this->entries.find(id);

	if (it != entries.end()) {
		log_error("%s: Entry with name %s already exists.\n", __func__,
			entry->getName().c_str());
		return false;
	}

	entries.insert(std::make_pair(id, entry));
	return true;
}

ConfigurationEntry *Configuration::getEntry(const unsigned int id)
{
	std::map<const unsigned int, ConfigurationEntry *>::iterator it =
		this->entries.find(id);

	if (it == entries.end())
		return nullptr;
	else
		return it->second;
}

std::string & Configuration::getName(void)
{
	return this->name;
}

bool Configuration::setValue(const unsigned int id, unsigned int value)
{
	ConfigurationEntry *entry = getEntry(id);

	if (entry == nullptr)
		return false;

	return entry->setValue(value);
}

bool Configuration::getValue(const unsigned int id, unsigned int & value)
{
	ConfigurationEntry *entry = getEntry(id);

	if (entry == nullptr)
		return false;

	value = entry->getValue();
	return true;
}
