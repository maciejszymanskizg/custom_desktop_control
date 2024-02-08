#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include "UART.h"
#include "Logger.h"

speed_t BaudrateToSpeed(UART::Baudrate baudrate)
{
	speed_t speed = B0;

	switch (baudrate)
	{
		case UART::Baudrate::BAUDRATE_9600:
			speed = B9600;
			break;
		case UART::Baudrate::BAUDRATE_19200:
			speed = B19200;
			break;
		case UART::Baudrate::BAUDRATE_38400:
			speed = B38400;
			break;
		case UART::Baudrate::BAUDRATE_57600:
			speed = B57600;
			break;
		case UART::Baudrate::BAUDRATE_115200:
			speed = B115200;
			break;
		case UART::Baudrate::BAUDRATE_230400:
			speed = B230400;
			break;
		case UART::Baudrate::BAUDRATE_460800:
			speed = B460800;
			break;
		default:
			break;
	}

	return speed;
}

UART::UART(const std::string & device, UART::Baudrate baudrate)
{
	fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		log_error("Could not open UART device [%s] : %s\n", device.c_str(), strerror(errno));
	} else {
		struct termios options;

		fcntl(fd, F_SETFL, O_RDWR);
		tcgetattr(fd, &options);

		cfmakeraw(&options);
		cfsetispeed(&options, BaudrateToSpeed(baudrate));
		cfsetospeed(&options, BaudrateToSpeed(baudrate));

		options.c_cflag |= (CLOCAL | CREAD);
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;
		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		options.c_oflag &= ~OPOST;
		options.c_cc [VMIN] = 0;
		options.c_cc [VTIME] = 5;
		tcsetattr (fd, TCSANOW, &options);
	}
}

UART::~UART()
{
	if (fd != -1)
		close(fd);
}

ICommunicationHandler::HandlerType UART::getHandlerType(void)
{
	return ICommunicationHandler::HandlerType::COMMUNICATION_HANDLER_UART;
}

ssize_t UART::readData(uint8_t *buffer, size_t size)
{
	ssize_t res = read(fd, (void *) buffer, size);

	if (res == -1) {
		log_error("Error in reading UART data : %s\n", strerror(errno));
	}

	return res;
}

ssize_t UART::writeData(const uint8_t *buffer, size_t size)
{
	ssize_t res = write(fd, (void *) buffer, size);

	if (res == -1) {
		log_error("Error in writing UART data : %s\n", strerror(errno));
	}

	return res;
}
