#include <unistd.h>
#include <errno.h>
#include "FDOperations.h"
#include "Logger.h"

FDOperations::FDOperations()
{
}

FDOperations::~FDOperations()
{
}

ssize_t FDOperations::readFDData(int fd, uint8_t *buffer, size_t size)
{
	ssize_t total_bytes = 0;
	ssize_t res;

	while (total_bytes < (ssize_t) size) {
		res = read(fd, (void *) &buffer[total_bytes], size - total_bytes);

		if (res >= 0) {
			total_bytes += res;
		} else if ((res == -1) && (errno != EAGAIN)) {
			log_error("Error in reading TCPIP data (%d).\n", errno);
			break;
		}
	}

	return total_bytes;
}

ssize_t FDOperations::writeFDData(int fd, const uint8_t *buffer, size_t size)
{
	ssize_t total_bytes = 0;
	ssize_t res;

	while (total_bytes < (ssize_t) size) {
		res = write(fd, (void *) &buffer[total_bytes], size - total_bytes);

		if (res >= 0) {
			total_bytes += res;
		} else if ((res == -1) && (errno != EAGAIN)) {
			log_error("Error in reading TCPIP data (%d).\n", errno);
			break;
		}
	}

	return total_bytes;
}
