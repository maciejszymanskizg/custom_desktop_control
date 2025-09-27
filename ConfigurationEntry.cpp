#include "ConfigurationEntry.h"
#include "Logger.h"
#include <string.h>

ConfigurationEntry::ConfigurationEntry(const char *name, uint32_t min_value, uint32_t max_value, uint32_t init_value)
{
	this->min_value = min_value;
	this->max_value = max_value;
	this->curr_value = init_value;
	this->prev_value = init_value;
	this->name = name;
	this->group_id = GROUP_ID_UNKNOWN;
	this->update_flag = false;
	this->group_update_flag = false;
}

ConfigurationEntry::~ConfigurationEntry()
{
}

const char *ConfigurationEntry::getName(void)
{
	return this->name;
}

uint32_t ConfigurationEntry::getValue(void)
{
	return this->curr_value;
}

uint32_t ConfigurationEntry::getMaxValue(void)
{
	return this->max_value;
}

uint32_t ConfigurationEntry::getPrevValue(void)
{
	return this->prev_value;
}

bool ConfigurationEntry::setValue(uint32_t value)
{
	if ((value < this->min_value) || (value > this->max_value)) {
		log_error("Configuration [%s] value %u out of bounds [%u, %u].\n",
				this->name, value, this->min_value, this->max_value);
		return false;
	}

	if (value != this->curr_value) {
		this->prev_value = this->curr_value;
		this->curr_value = value;
		this->update_flag = true;
		this->group_update_flag = true;
	}

	return true;
}

bool ConfigurationEntry::isUpdated(void)
{
	return this->update_flag;
}

void ConfigurationEntry::cleanUpdate(void)
{
	this->update_flag = false;
}

uint32_t ConfigurationEntry::getGroupId(void)
{
	return this->group_id;
}

void ConfigurationEntry::setGroupId(uint32_t id)
{
	this->group_id = id;
}

bool ConfigurationEntry::isGroupUpdated(void)
{
	return this->group_update_flag;
}

void ConfigurationEntry::cleanGroupUpdate(void)
{
	this->group_update_flag = false;
}
