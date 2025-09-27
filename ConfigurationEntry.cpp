#include "ConfigurationEntry.h"
#include "Logger.h"
#include <string.h>

ConfigurationEntry::ConfigurationEntry(uint32_t id, const char *name, uint32_t min_value, uint32_t max_value, uint32_t init_value)
{
	this->id = id;
	this->name = name;
	this->min_value = min_value;
	this->max_value = max_value;
	this->curr_value = init_value;
	this->prev_value = init_value;
	this->flags = 0;
	this->is_updated = false;
}

ConfigurationEntry::~ConfigurationEntry()
{
}

ConfigurationEntry::ConfigurationEntry(const ConfigurationEntry &other)
{
	this->id = other.id;
	this->name = other.name;
	this->min_value = other.min_value;
	this->max_value = other.max_value;
	this->prev_value = other.prev_value;
	this->curr_value = other.curr_value;
	this->flags = other.flags;
	this->is_updated = other.is_updated;
}

ConfigurationEntry * ConfigurationEntry::clone(void)
{
	return new ConfigurationEntry(*this);
}

const uint32_t ConfigurationEntry::getId(void)
{
	return this->id;
}

const char *ConfigurationEntry::getName(void)
{
	return this->name;
}

uint32_t ConfigurationEntry::getValue(void)
{
	return this->curr_value;
}

uint32_t ConfigurationEntry::getMinValue(void)
{
	return this->min_value;
}

uint32_t ConfigurationEntry::getMaxValue(void)
{
	return this->max_value;
}

uint32_t ConfigurationEntry::getPrevValue(void)
{
	return this->prev_value;
}

uint32_t ConfigurationEntry::getFlags(void)
{
	return this->flags;
}

bool ConfigurationEntry::setValue(uint32_t value)
{
	return setValue(value, UpdateSource::CONFIGURATION_ENTRY_UPDATE_SOURCE_EXTERNAL);
}

bool ConfigurationEntry::setValue(uint32_t value, enum UpdateSource source)
{
	bool result = false;

	if ((value < this->min_value) || (value > this->max_value)) {
		log_error("Configuration [%s] value %u out of bounds [%u, %u].\n",
				this->name, value, this->min_value, this->max_value);
	} else if (value == this->prev_value) {
		//log_debug("Configuration [%s] same as previous [%u]\n", this->name, value);
	} else {
		this->prev_value = this->curr_value;
		this->curr_value = value;

		/* mark as updated only if update comes from external source */
		if (source == CONFIGURATION_ENTRY_UPDATE_SOURCE_EXTERNAL)
			this->is_updated = true;

		result = true;
	}

	return result;
}

void ConfigurationEntry::setFlags(uint32_t flags)
{
	this->flags = flags;
}

bool ConfigurationEntry::isUpdated(void)
{
	return this->is_updated;
}

void ConfigurationEntry::cleanUpdate(void)
{
	this->is_updated = false;
}
