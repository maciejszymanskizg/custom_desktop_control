#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <list>
#include "JSON.h"
#include "TrainConfigurationIDs.h"
#include "TrainConfigurationJson.h"
#include "Logger.h"

#define JSON_KEY_VALUE_INTERNAL_BUFFER_SIZE 64

bool JSON::checkJson(const char *str)
{
	/* simple json check should count number of '{' and '}'
	 * any additional checks are redundant as there should be a check
	 * on opposite site
	 */
	unsigned int i;
	unsigned int open_count = 0;
	unsigned int close_count = 0;

	for (i = 0; i < strlen(str); i++)
	{
		if (str[i] == '{' && i > 0 && str[i - 1] != '\\') {
			open_count++;
		} else if (str[i] == '}' && i > 0 && str[i - 1] != '\\')
		{
			close_count++;
		}

	}

	return (open_count != 0 && open_count == close_count);
}

bool JSON::parseKeyValue(char *keyValue, Configuration *conf)
{
	bool result = false;
	char *charValue;
	char *key = strtok(keyValue, ":");

	if (key != NULL) {
		/* remove quotation */
		if (key[0] == '\"') {
			key++;
			if (key[strlen(key)] == '\"')
				key[strlen(key)] = '\0';
		}

		charValue = strtok(NULL, ":");
		if (charValue != NULL)
		{
			unsigned int value = (unsigned int) atoi(charValue);
			conf->setValue(TrainConfigurationJson::JsonKeyToId(key), value);
			result = true;
		}
	}

	return result;
}

bool JSON::parseJson(const char *str, Configuration *conf)
{
	bool result = true;
	/* at first, let's get rid of first and last '{' and '}' */
	char *json_string = (char *) str;
	json_string++;
	json_string[strlen(json_string) - 1] = '\0';

	char *token = strtok(json_string, ",");

	while (token != NULL)
	{
		if ((result = parseKeyValue(token, conf)) == false)
			break;

		token = strtok(NULL, ",");
	}

	return result;
}

bool JSON::ConfigurationToJson(Configuration *conf, char *buffer, unsigned int max_size)
{
	bool result = false;
	char key_value_buffer[JSON_KEY_VALUE_INTERNAL_BUFFER_SIZE];
	unsigned int key_value_length;
	unsigned int entries_count = 0;
	unsigned int index = 0;

	if (!buffer || !max_size)
		return false;

	bzero(buffer, max_size);
	buffer[index++] = '{';

	for (unsigned int i = 0; i < CONFIGURATION_ID_MAX; i++)
	{
		const char *key = TrainConfigurationJson::IdToJsonKey(i);
		if (conf->isUpdated(i)) {
			if (key == NULL) {
				conf->cleanUpdate(i);
				continue;
			}

			/* at first - create json entry in key_value_buffer */
			snprintf(key_value_buffer, JSON_KEY_VALUE_INTERNAL_BUFFER_SIZE,
					"%s\"%s\":%u", entries_count ? "," : "", key, conf->getValue(i));
			conf->cleanUpdate(i);

			/* check if we have enough space to add it into buffer */
			key_value_length = strlen(key_value_buffer);
			if (key_value_length + 2 < max_size - index)
			{
				strcat(&buffer[index], key_value_buffer);
				index += strlen(key_value_buffer);
				entries_count++;
			}
			else
			{
				log_error("Not enough space in buffer to add key-value entry %s\n", key_value_buffer);
				goto out;
			}
		}
	}

	buffer[index++] = '}';
	buffer[index] = '\0';

	result = true;

out:
	return result;
}

bool JSON::JsonToConfiguration(const char *str, Configuration *conf)
{
	bool result = false;

	if (strlen(str) == 0)
	{
		log_debug("Empty input string\n");
		goto out;
	}
	else if (!checkJson(str))
	{
		log_debug("JSON check failed\n");
		goto out;
	}

	result = parseJson(str, conf);

out:
	return result;
}
