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

UART::UART(const char *device, UART::Baudrate baudrate, volatile sig_atomic_t *flag) : FDOperations(flag)
{
	struct termios options;
	int res;

	this->fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

	if (this->fd == -1) {
		log_error("Could not open UART device [%s] - errno %d\n", device, errno);
		goto exit;
	}

	if (tcgetattr(this->fd, &options) != 0) {
		log_error("Could not get UART port [%s] settings - errno %d\n", device, errno);
		goto fd_close;
	}

	res = fcntl(this->fd, F_GETFL, 0);

	if (res == -1) {
		log_error("Could not get UART port [%s] flags - errno %d\n", device, errno);
		goto fd_close;
	}

	if (fcntl(this->fd, F_SETFL, res | O_NONBLOCK) == -1) {
		log_error("Could not set UART port [%s] flags - errno %d\n", device, errno);
		goto fd_close;
	}

	options.c_cflag &= ~PARENB; /* clear parity bit */
	options.c_cflag &= ~CSTOPB; /* clear stop field */
	options.c_cflag &= ~CSIZE; /* clear all size bits */
	options.c_cflag |= CS8; /* set 8 bits per byte */
	options.c_cflag &= ~CRTSCTS; /* disable RTS/CTS hardware flow control */
	options.c_cflag |= CLOCAL; /* disables carrier detect */
	options.c_cflag |= CREAD; /* enable UART read */

	options.c_lflag &= ~ICANON; /* disable canonical mode */
	options.c_lflag &= ~ECHO; /* disable echo */
	options.c_lflag &= ~ISIG; /* disable interpretation of INTR, QUIT and SUSP */

	options.c_iflag &= ~(IXON | IXOFF | IXANY); /* disable software flow control */
	options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); /* disable special handling of received bytes */

	options.c_oflag &= ~OPOST; /* prevent special interpretation of output bytes */
	options.c_oflag &= ~ONLCR; /* prevent new line to carriage return conversion */

	/* set timeoout to 0.2 seconds */
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 2;

	/* set baudrate */
	cfsetispeed(&options, BaudrateToSpeed(baudrate));
	cfsetospeed(&options, BaudrateToSpeed(baudrate));

	if (tcsetattr(this->fd, TCSANOW, &options) != 0) {
		log_error("Could not get UART port [%s] settings - errno %d\n", device, errno);
		goto fd_close;
	}

	goto exit;

fd_close:
	close(this->fd);
	this->fd = -1;
exit:
	return;
}

UART::~UART()
{
	closeConnection();
}

void UART::closeConnection(void)
{

	if (this->fd != -1) {
		close(this->fd);
		this->fd = -1;
	}
}

ICommunicationHandler::HandlerType UART::getHandlerType(void)
{
	return ICommunicationHandler::HandlerType::COMMUNICATION_HANDLER_UART;
}

ssize_t UART::readData(uint8_t *buffer, size_t size)
{
	ssize_t result = -1;

	if (this->fd != -1)
		result = readFDData(this->fd, buffer, size);

	return result;
}

ssize_t UART::writeData(const uint8_t *buffer, size_t size)
{
	ssize_t result = -1;

	if (this->fd != -1)
		result = writeFDData(this->fd, buffer, size);

	if (result != -1)
		tcdrain(this->fd);

	return result;
}
