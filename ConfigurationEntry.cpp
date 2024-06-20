#include "ConfigurationEntry.h"
#include "Logger.h"
#include <string.h>

ConfigurationEntry::ConfigurationEntry(const char *name, uint32_t min_value, uint32_t max_value, uint32_t init_value)
{
	//log_debug("%s() %p\n", __func__, this);
	this->min_value = min_value;
	this->max_value = max_value;
	this->curr_value = init_value;
	this->prev_value = init_value;
	this->name = name;
}

ConfigurationEntry::~ConfigurationEntry()
{
	//log_debug("%s() %p\n", __func__, this);
}

const char *ConfigurationEntry::getName(void)
{
	return this->name;
}

uint32_t ConfigurationEntry::getValue(void)
{
	return this->curr_value;
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

	this->prev_value = this->curr_value;
	this->curr_value = value;

	return true;
}

bool ConfigurationEntry::isUpdated(void)
{
	return (bool) (this->curr_value != this->prev_value);
}

void ConfigurationEntry::cleanUpdate(void)
{
	this->prev_value = this->curr_value;
}
