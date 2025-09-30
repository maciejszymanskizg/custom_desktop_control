#include <utility>
#include <string.h>
#include "Configuration.h"
#include "Logger.h"

Configuration::Configuration(const char *name, uint32_t size)
{
	this->name = name;
	this->external_configuration = nullptr;
	this->entries = new ConfigurationEntry *[size];
	this->mutex = new Mutex();
	if (this->entries != nullptr) {
		for (uint32_t i = 0; i < size; i++)
			this->entries[i] = nullptr;

		this->entries_count = size;
	} else {
		log_error("Could not allocate memory for %u configuration entries\n", size);
	}
}

Configuration::Configuration(Configuration & other, const char *name)
{
	this->name = name;
	this->external_configuration = nullptr;
	this->mutex = new Mutex();
	this->entries = new ConfigurationEntry *[other.entries_count];
	this->entries_count = other.entries_count;

	if (this->entries != nullptr) {
		for (uint32_t i = 0; i < this->entries_count; i++) {
			this->entries[i] = other.entries[i]->clone();
		}
	}
}

Configuration::~Configuration()
{
	for (uint32_t i = 0; i < this->entries_count; i++) {
		if (this->entries[i] != nullptr) {
			ConfigurationEntry *entry = this->entries[i];
			delete entry;
		}
	}

	delete [] this->entries;
	delete this->mutex;
}

Configuration * Configuration::clone(const char *name)
{
	return new Configuration(*this, name);
}

void Configuration::addExternalConfiguration(Configuration *conf)
{
	this->external_configuration = conf;
}

bool Configuration::addConfigurationEntry(const uint32_t id, ConfigurationEntry *entry)
{
	if ((id >= this->entries_count) || (entry == nullptr))
		return false;

	if (this->entries[id] != nullptr) {
		log_error("%s: Entry with name %s already exists.\n", __func__,
			entry->getName());
		return false;
	}

	this->entries[id] = entry;
	return true;
}

void Configuration::accessLock(void)
{
	this->mutex->lock();
}

void Configuration::accessUnlock(void)
{
	this->mutex->unlock();
}

const char *Configuration::getName(void)
{
	return this->name;
}
ConfigurationEntry *Configuration::getEntry(const uint32_t id)
{
	return (id < this->entries_count ? this->entries[id] : nullptr);
}

bool Configuration::getValue(const uint32_t id, uint32_t & value)
{
	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		value = this->entries[id]->getValue();
		return true;
	}

	return false;
}

uint32_t Configuration::getValue(const uint32_t id)
{
	uint32_t value = CONFIGURATION_INVALID_VALUE;

	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		value = this->entries[id]->getValue();
	}

	return value;
}

uint32_t Configuration::getMaxValue(const uint32_t id)
{
	uint32_t value = CONFIGURATION_INVALID_VALUE;

	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		value = this->entries[id]->getMaxValue();
	}

	return value;
}

bool Configuration::setValue(const uint32_t id, uint32_t value)
{
	return this->setValue(id, value, CONFIGURATION_UPDATE_SOURCE_INTERNAL);
}

bool Configuration::setValue(const uint32_t id, uint32_t value, enum UpdateSource source)
{
	bool result = false;
	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		ConfigurationEntry::UpdateSource src = (source == CONFIGURATION_UPDATE_SOURCE_EXTERNAL ?
				ConfigurationEntry::UpdateSource::CONFIGURATION_ENTRY_UPDATE_SOURCE_EXTERNAL :
				ConfigurationEntry::UpdateSource::CONFIGURATION_ENTRY_UPDATE_SOURCE_INTERNAL);
		result = this->entries[id]->setValue(value, src);

		if ((result == true) && (source == CONFIGURATION_UPDATE_SOURCE_INTERNAL) && (this->external_configuration)) {
			external_configuration->accessLock();
			external_configuration->setValue(id, value, Configuration::CONFIGURATION_UPDATE_SOURCE_EXTERNAL);
			external_configuration->accessUnlock();
		}
	}

	return result;

}

bool Configuration::isUpdated(const uint32_t id)
{
	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		return this->entries[id]->isUpdated();
	}

	return false;
}

bool Configuration::checkUpdates(const uint32_t *ids, uint32_t elems)
{
	bool result = false;

	for (uint32_t i = 0; i < (uint32_t) elems; i++) {
		if ((ids[i] < this->entries_count) &&
				(this->entries[ids[i]] != nullptr) &&
				(this->entries[ids[i]]->isUpdated())) {
			result = true;
			break;
		}
	}

	return result;
}

bool Configuration::checkUpdates(const uint32_t flags)
{
	bool result = false;

	for (uint32_t i = 0; i < this->entries_count; i++) {
		if (((this->entries[i]->getFlags() & flags) == flags) &&
				(this->entries[i]->isUpdated())) {
			result = true;
			break;
		}
	}

	return result;
}

void Configuration::cleanUpdate(const uint32_t id)
{
	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		this->entries[id]->cleanUpdate();
	}
}

void Configuration::cleanUpdates(void)
{
	for (uint32_t i = 0; i < this->entries_count; i++) {
		if ((this->entries[i] != nullptr) && (this->entries[i]->isUpdated())) {
			this->entries[i]->cleanUpdate();
		}
	}
}

void Configuration::cleanUpdates(const uint32_t flags)
{
	for (uint32_t i = 0; i < this->entries_count; i++) {
		if (((this->entries[i]->getFlags() & flags) == flags) &&
				(this->entries[i]->isUpdated())) {
			this->entries[i]->cleanUpdate();
		}
	}
}

bool Configuration::setFlags(const uint32_t id, const uint32_t flags)
{
	bool result = false;

	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		this->entries[id]->setFlags(flags);
		result = true;
	}

	return result;
}

bool Configuration::addFlags(const uint32_t id, const uint32_t flags)
{
	bool result = false;

	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		this->entries[id]->setFlags(this->entries[id]->getFlags() | flags);
		result = true;
	}

	return result;

}

bool Configuration::getFlags(const uint32_t id, uint32_t & flags)
{
	bool result = false;

	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		flags = this->entries[id]->getFlags();
		result = true;
	}

	return result;
}

uint32_t Configuration::getFlags(const uint32_t id)
{
	uint32_t result = 0;

	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		result = this->entries[id]->getFlags();
	}

	return result;
}

void Configuration::dumpConfig(void)
{
	for (uint32_t i = 0; i < this->entries_count; i++) {
		if (this->entries[i] != nullptr) {
			log_debug("[0x%08x] [%s] : %u\n", i, this->entries[i]->getName(),
				this->entries[i]->getValue());
		}
	}
}

bool Configuration::dumpConfigUpdates(void)
{
	bool result = false;

	for (uint32_t i = 0; i < this->entries_count; i++) {
		if ((this->entries[i] != nullptr) && (this->entries[i]->isUpdated())) {
			log_info("(%s) [0x%08x] [%s] : %u\n", this->name, i, this->entries[i]->getName(),
				this->entries[i]->getValue());

			result = true;
		}
	}

	return result;
}


