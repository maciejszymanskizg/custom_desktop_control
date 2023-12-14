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

I2C::I2C(const std::string & path)
{
    fd = open(path.c_str(), O_RDWR);

    if (fd == -1) {
        log_error("Could not open i2c device [%s] : %s\n", path.c_str(), strerror(errno));
    }
}

I2C::~I2C()
{
    if (fd != -1)
        close(fd);
}

int I2C::setSlaveAddr(uint8_t addr)
{
    int res = ioctl(fd, I2C_SLAVE, addr >> 1);

    if (res == -1) {
        log_error("Could not set slave address to 0x%x : %s\n", addr, strerror(errno));
    }

    return res;
}

ICommunicationHandler::HandlerType I2C::getHandlerType(void)
{
    return ICommunicationHandler::HandlerType::COMMUNICATION_HANDLER_I2C;
}

ssize_t I2C::readData(uint8_t *buffer, size_t length)
{
    return read(fd, (void *) buffer, length);
}

ssize_t I2C::writeData(const uint8_t *buffer, size_t length)
{
    return write(fd, (const void *) buffer, length);
}
