#ifndef JSON_H
#define JSON_H

#include "Configuration.h"

class JSON {
	public:
		static bool JsonToConfiguration(const char *str, Configuration *conf);
		static bool ConfigurationToJson(Configuration *conf, char *buffer, unsigned int max_size);
	private:
		static bool checkJson(const char *str);
		static bool parseJson(const char *str, Configuration *conf);
		static bool parseKeyValue(char *keyValue, Configuration *conf);
};

#endif /* JSON_H */
