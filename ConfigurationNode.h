#ifndef CONFIGURATION_NODE_H
#define CONIFGURATION_NODE_H

#include <stdint.h>

struct ConfigurationNode {
	private:
		uint32_t id;
		const char *name;
		uint32_t min;
		uint32_t max;
		uint32_t init;

	public:
		ConfigurationNode(uint32_t id, const char *name, uint32_t min, uint32_t max, uint32_t init);
		~ConfigurationNode();
		uint32_t getId();
		const char *getName();
		uint32_t getMin();
		uint32_t getMax();
		uint32_t getInit();
};

#endif /* CONIFGURATION_NODE_H */
