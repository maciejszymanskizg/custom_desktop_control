#include <utility>
#include "Configuration.h"
#include "Logger.h"

Configuration::Configuration(const std::string & name)
{
	this->name = name;
}

Configuration::~Configuration()
{
	std::map<const uint32_t, ConfigurationEntry *>::iterator it;
	ConfigurationEntry *e;

	while (entries.size() > 0) {
		e = entries.begin()->second;
		entries.erase(entries.begin());
		delete e;
	}
}

bool Configuration::addConfigurationEntry(const uint32_t id, ConfigurationEntry *entry)
{
	if (entry == nullptr)
		return false;

	std::map<const uint32_t, ConfigurationEntry *>::iterator it =
		this->entries.find(id);

	if (it != entries.end()) {
		log_error("%s: Entry with name %s already exists.\n", __func__,
			entry->getName().c_str());
		return false;
	}

	entries.insert(std::make_pair(id, entry));
	return true;
}

ConfigurationEntry *Configuration::getEntry(const uint32_t id)
{
	std::map<const uint32_t, ConfigurationEntry *>::iterator it =
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

bool Configuration::setValue(const uint32_t id, uint32_t value)
{
	ConfigurationEntry *entry = getEntry(id);

	if (entry == nullptr)
		return false;

	return entry->setValue(value);
}

bool Configuration::getValue(const uint32_t id, uint32_t & value)
{
	ConfigurationEntry *entry = getEntry(id);

	if (entry == nullptr)
		return false;

	value = entry->getValue();
	return true;
}

uint32_t Configuration::getValue(const uint32_t id)
{
	ConfigurationEntry *entry = getEntry(id);

	return (entry ? entry->getValue() : CONFIGURATION_INVALID_VALUE);
}

bool Configuration::isUpdated(const uint32_t id)
{
	ConfigurationEntry *entry = getEntry(id);

	return (entry ? entry->isUpdated() : false);
}

void Configuration::accessLock(void)
{
	mutex.lock();
}

void Configuration::accessUnlock(void)
{
	mutex.unlock();
}

void Configuration::dumpConfig(void)
{
	std::map<const uint32_t, ConfigurationEntry *>::iterator it;

	for (it = this->entries.begin(); it != this->entries.end(); it++) {
		log_debug("[0x%08x] [%s] : %u\n", it->first, it->second->getName().c_str(),
				it->second->getValue());
	}
}

bool Configuration::dumpConfigUpdates(void)
{
	bool result = false;
	std::map<const uint32_t, ConfigurationEntry *>::iterator it;

	for (it = this->entries.begin(); it != this->entries.end(); it++) {
		if (it->second->isUpdated()) {
			log_debug("[0x%08x] [%s] : %u -> %u\n", it->first, it->second->getName().c_str(),
					it->second->getPrevValue(), it->second->getValue());

			result = true;
		}
	}

	return result;
}

void Configuration::cleanUpdates(const uint32_t id)
{
	std::map<const uint32_t, ConfigurationEntry *>::iterator it = this->entries.find(id);

	if ((it != this->entries.end()) && (it->second->isUpdated())) {
		it->second->cleanUpdate();
	}
}

void Configuration::cleanUpdates(void)
{
	std::map<const uint32_t, ConfigurationEntry *>::iterator it;

	for (it = this->entries.begin(); it != this->entries.end(); it++) {
		if (it->second->isUpdated()) {
			it->second->cleanUpdate();
		}
	}
}

bool Configuration::checkUpdates(const uint32_t *ids, size_t elems)
{
	bool result = false;
	std::map<const uint32_t, ConfigurationEntry *>::iterator it;

	for (uint32_t i = 0; i < (uint32_t) elems; i++) {
		it = this->entries.find(ids[i]);

		if ((it != this->entries.end()) && (it->second->isUpdated())) {
			result = true;
			break;
		}
	}

	return result;
}
