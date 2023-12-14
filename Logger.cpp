#include <iostream>
#include <stdarg.h>
#include "Logger.h"

Logger::Logger()
{
}

Logger & Logger::getInstance()
{
	static Logger instance;
	return instance;
}

void Logger::log(Logger::Severity severity, const char *fmt, ...)
{
	char buffer[128];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	switch (severity)
	{
		case ERROR:
			std::cout << Logger::color_red;
			break;
		case Logger::WARNING:
			std::cout << Logger::color_orange;
			break;
		case Logger::INFO:
			std::cout << Logger::color_green;
			break;
		case Logger::DEBUG:
			std::cout << Logger::color_blue;
			break;
		default:
			break;
	}

	std::cout << buffer << Logger::color_none;
}
