#ifndef DUMMY_OUTPUT_CONTROLLER_H
#define DUMMY_OUTPUT_CONTROLLER_H

#include "IDummyController.h"

class DummyOutputController : public IDummyController {
	public:
		DummyOutputController(Configuration *conf);
		virtual ~DummyOutputController();

		virtual void sync(SyncDirection direction);
};

#endif /* DUMMY_OUTPUT_CONTROLLER_H */
