#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

class Logger
{
	private:
		const char *color_none = "\033[0m";
		const char *color_red = "\033[1;31m";
		const char *color_green = "\033[1;32m";
		const char *color_orange = "\033[1;33m";
		const char *color_blue = "\033[1;34m";

		Logger();

	public:
		enum Severity
		{
			ERROR,
			WARNING,
			INFO,
			DEBUG
		};

		Logger(const Logger &obj) = delete;
		static Logger & getInstance();
		void log(Severity severity, const char *format, ...);
};

#define log_error(fmt, ...) Logger::getInstance().log(Logger::Severity::ERROR, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...) Logger::getInstance().log(Logger::Severity::WARNING, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) Logger::getInstance().log(Logger::Severity::INFO, fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...) Logger::getInstance().log(Logger::Severity::DEBUG, fmt, ##__VA_ARGS__)


#endif /* LOGGER_H */
