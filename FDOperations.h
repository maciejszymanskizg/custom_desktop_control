#ifndef FD_OPERATIONS_H
#define FD_OPERATIONS_H

#include <stdint.h>
#include <signal.h>

class FDOperations {
	public:
		enum BlockingMode {
			FDOPERATIONS_READ_MODE_BLOCKING,
			FDOPERATIONS_READ_MODE_NON_BLOCKING
		};

	private:
		volatile sig_atomic_t *running_flag;
		bool isFDDataAvailale(int fd);
		enum BlockingMode blocking_mode;

	public:
		FDOperations();
		FDOperations(volatile sig_atomic_t *flag);
		virtual ~FDOperations();

		void setBlockingMode(enum BlockingMode blocking_mode);

		ssize_t readFDData(int fd, uint8_t *buffer, size_t size);
		ssize_t writeFDData(int fd, const uint8_t *buffer, size_t size);
};

#endif /* FD_OPERATIONS_H */
