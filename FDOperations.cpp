#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include "FDOperations.h"
#include "Logger.h"

FDOperations::FDOperations()
{
	this->running_flag = nullptr;
	this->blocking_mode = FDOPERATIONS_READ_MODE_BLOCKING;
}

FDOperations::FDOperations(volatile sig_atomic_t *flag)
{
	this->running_flag = flag;
	this->blocking_mode = FDOPERATIONS_READ_MODE_BLOCKING;
}

FDOperations::~FDOperations()
{
}

void FDOperations::setBlockingMode(enum BlockingMode mode)
{
	this->blocking_mode = mode;
}

bool FDOperations::isFDDataAvailale(int fd)
{
	bool result = false;
	fd_set read_fds;
	struct timeval tv;

	FD_ZERO(&read_fds);
	FD_SET(fd, &read_fds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if (select(fd + 1, &read_fds, NULL, NULL, &tv) > 0 && (FD_ISSET(fd, &read_fds)))
		result = true;

	return result;
}

ssize_t FDOperations::readFDData(int fd, uint8_t *buffer, size_t size)
{
	ssize_t total_bytes = 0;
	ssize_t res;

	if ((this->blocking_mode == FDOPERATIONS_READ_MODE_NON_BLOCKING) &&
			(!isFDDataAvailale(fd)))
		goto out;

	while (total_bytes < (ssize_t) size) {
		res = read(fd, (void *) &buffer[total_bytes], size - total_bytes);

		if (res > 0) {
			total_bytes += res;
		} else if (res == 0) {
			/* no bytes written - connection could be closed by peer */
			total_bytes = -1;
			break;
		} else if ((res == -1) && (errno != EAGAIN)) {
			log_error("Error in reading FD data (%d).\n", errno);
			total_bytes = -1;
			break;
		}

		if ((running_flag != nullptr) && (*running_flag == 0))
			break;
	}

out:
	return total_bytes;
}

ssize_t FDOperations::writeFDData(int fd, const uint8_t *buffer, size_t size)
{
	ssize_t total_bytes = 0;
	ssize_t res;

	while (total_bytes < (ssize_t) size) {
		res = write(fd, (void *) &buffer[total_bytes], size - total_bytes);

		if (res > 0) {
			total_bytes += res;
		} else if (res == 0) {
			/* no bytes written - connection could be closed by peer */
			total_bytes = -1;
			break;
		} else if ((res == -1) && (errno != EAGAIN)) {
			log_error("Error in writing FD data (%d).\n", errno);
			total_bytes = -1;
			break;
		}

		if ((running_flag != nullptr) && (*running_flag == 0))
			break;
	}

	return total_bytes;
}
