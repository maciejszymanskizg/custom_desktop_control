#include <utility>
#include <string.h>
#include "Configuration.h"
#include "Logger.h"

Configuration::Configuration(const char *name, uint32_t size)
{
	//log_debug("%s() %p\n", __func__, this);
	this->name = name;
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

Configuration::~Configuration()
{
	//log_debug("%s() %p\n", __func__, this);
	for (uint32_t i = 0; i < this->entries_count; i++) {
		if (this->entries[i] != nullptr) {
			ConfigurationEntry *entry = this->entries[i];
			delete entry;
		}
	}
	delete [] this->entries;
	delete this->mutex;
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

ConfigurationEntry *Configuration::getEntry(const uint32_t id)
{
	return (id < this->entries_count ? this->entries[id] : nullptr);
}

const char *Configuration::getName(void)
{
	return this->name;
}

bool Configuration::setValue(const uint32_t id, uint32_t value)
{
	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		return this->entries[id]->setValue(value);
	}

	return false;
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

bool Configuration::isUpdated(const uint32_t id)
{
	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		return this->entries[id]->isUpdated();
	}

	return false;
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
			log_debug("[0x%08x] [%s] : %u\n", i, this->entries[i]->getName(),
				this->entries[i]->getValue());

			result = true;
		}
	}

	return result;
}

void Configuration::cleanUpdates(const uint32_t id)
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

bool Configuration::checkGroupUpdates(uint32_t group_id)
{
	bool result = false;

	for (uint32_t i = 0; i < this->entries_count; i++) {
		if ((this->entries[i]->getGroupId() == group_id) &&
				(this->entries[i]->isGroupUpdated())) {
			result = true;
			break;
		}
	}

	return result;
}

void Configuration::cleanGroupUpdates(const uint32_t group_id)
{
	for (uint32_t i = 0; i < this->entries_count; i++) {
		if (this->entries[i]->getGroupId() == group_id) {
			this->entries[i]->cleanGroupUpdate();
		}
	}
}

void Configuration::dumpPointers(void)
{
	for (uint32_t i = 0; i < (uint32_t) this->entries_count; i++) {
		log_info("this->entries[%u] = %p\n", i,
				this->entries[i]);
	}
}

void Configuration::accessLock(void)
{
	this->mutex->lock();
}

void Configuration::accessUnlock(void)
{
	this->mutex->unlock();
}

bool Configuration::setGroupId(const uint32_t id, const uint32_t group_id)
{
	bool result = false;

	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		this->entries[id]->setGroupId(group_id);
		result = true;
	}

	return result;
}

bool Configuration::getGroupId(const uint32_t id, uint32_t & group_id)
{
	bool result = false;

	if ((id < this->entries_count) && (this->entries[id] != nullptr)) {
		group_id = this->entries[id]->getGroupId();
		result = true;
	}

	return result;
}
