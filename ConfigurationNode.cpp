#include "ConfigurationNode.h"

ConfigurationNode::ConfigurationNode(uint32_t id, const char *name, uint32_t min, uint32_t max, uint32_t init)
{
	this->id = id;
	this->name = name;
	this->min = min;
	this->max = max;
	this->init = init;
}

ConfigurationNode::~ConfigurationNode()
{
}

uint32_t ConfigurationNode::getId()
{
	return this->id;
}

const char *ConfigurationNode::getName()
{
	return this->name;
}

uint32_t ConfigurationNode::getMin()
{
	return this->min;
}

uint32_t ConfigurationNode::getMax()
{
	return this->max;
}

uint32_t ConfigurationNode::getInit()
{
	return this->init;
}
