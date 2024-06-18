#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "I2C.h"
#include "Logger.h"

I2C::I2C(const char *path)
{
	fd = open(path, O_RDWR);

	if (fd == -1) {
		log_error("Could not open i2c device [%s] : %s\n", path, strerror(errno));
	}

	this->use_smbus = false;
}

I2C::~I2C()
{
	if (fd != -1)
		close(fd);
}

int I2C::setSlaveAddr(uint8_t addr)
{
	int res = ioctl(fd, I2C_SLAVE, addr);

	if (res == -1) {
		log_error("Could not set slave address to 0x%x : %s\n", addr, strerror(errno));
	}

	return res;
}

void I2C::setSMBus(bool enable)
{
	this->use_smbus = enable;
}

ICommunicationHandler::HandlerType I2C::getHandlerType(void)
{
	return ICommunicationHandler::HandlerType::COMMUNICATION_HANDLER_I2C;
}

ssize_t I2C::readSMBusData(uint8_t *buffer, size_t length)
{
	ssize_t result = -1;

	if (length <= I2C_SMBUS_BLOCK_MAX) {
		struct i2c_smbus_ioctl_data args;

		args.read_write = I2C_SMBUS_READ;
		args.command = 0;
		args.size = (length == 1 ? I2C_SMBUS_BYTE_DATA : (length == 2 ?
					I2C_SMBUS_WORD_DATA : I2C_SMBUS_BLOCK_DATA));
		args.data = (union i2c_smbus_data *) buffer;

		if (ioctl(fd, I2C_SMBUS, &args) == 0) {
			result = (ssize_t) length;
		}
	}

	return result;
}

ssize_t I2C::readData(uint8_t *buffer, size_t length)
{
	ssize_t result = 0;

	if (use_smbus)
		result = readSMBusData(buffer, length);
	else
		result = read(fd, (void *) buffer, length);

	return result;
}

ssize_t I2C::writeSMBusData(const uint8_t *buffer, size_t length)
{
	ssize_t result = -1;

	if (length <= I2C_SMBUS_BLOCK_MAX) {
		struct i2c_smbus_ioctl_data args;

		args.read_write = I2C_SMBUS_WRITE;
		args.command = 0;
		args.size = (length == 1 ? I2C_SMBUS_BYTE_DATA : (length == 2 ?
					I2C_SMBUS_WORD_DATA : I2C_SMBUS_BLOCK_DATA));
		args.data = (union i2c_smbus_data *) buffer;

		if (ioctl(fd, I2C_SMBUS, &args) == 0) {
			result = (ssize_t) length;
		}
	}

	return result;
}

ssize_t I2C::writeData(const uint8_t *buffer, size_t length)
{
	ssize_t result = 0;

	if (use_smbus)
		result = writeSMBusData(buffer, length);
	else
    result = write(fd, (const void *) buffer, length);

	return result;
}
