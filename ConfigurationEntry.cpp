#include "ConfigurationEntry.h"

ConfigurationEntry::ConfigurationEntry(const std::string & name, unsigned int min_value, unsigned int max_value, unsigned int init_value)
{
	this->name = name;
	this->min_value = min_value;
	this->max_value = max_value;
	this->curr_value = init_value;
	this->prev_value = init_value;
}

std::string & ConfigurationEntry::getName(void)
{
	return this->name;
}

unsigned int ConfigurationEntry::getValue(void)
{
	return this->curr_value;
}

unsigned int ConfigurationEntry::getPrevValue(void)
{
	return this->prev_value;
}

bool ConfigurationEntry::setValue(unsigned int value)
{
	if ((value < this->min_value) || (value > this->max_value))
		return false;

	this->prev_value = this->curr_value;
	this->curr_value = value;

	return true;
}
