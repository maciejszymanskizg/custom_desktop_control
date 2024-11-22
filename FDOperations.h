#ifndef FD_OPERATIONS_H
#define FD_OPERATIONS_H

#include <stdint.h>
#include <signal.h>

class FDOperations {
	private:
		volatile sig_atomic_t *running_flag;
	public:
		FDOperations();
		FDOperations(volatile sig_atomic_t *flag);
		virtual ~FDOperations();

		ssize_t readFDData(int fd, uint8_t *buffer, size_t size);
		ssize_t writeFDData(int fd, const uint8_t *buffer, size_t size);
};

#endif /* FD_OPERATIONS_H */
