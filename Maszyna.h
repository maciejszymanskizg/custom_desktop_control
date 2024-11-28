#ifndef MASZYNA_H
#define MASZYNA_H

#include "Configuration.h"

#define MASZYNA_INPUT_BUFFER_SIZE 52
#define MASZYNA_OUTPUT_BUFFER_SIZE 20

class Maszyna {
	private:
		Configuration *conf;

	public:
		Maszyna(Configuration *conf);
		~Maszyna();

		bool handleInputBuffer(uint8_t *buffer, uint32_t size);
		bool handleOutputBuffer(uint8_t *buffer, uint32_t size);

		void dumpBuffer(bool input_buffer, uint8_t *buffer, uint32_t size);
};

#endif /* MASZYNA_H */
