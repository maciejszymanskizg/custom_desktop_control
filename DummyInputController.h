#ifndef DUMMY_INPUT_CONTROLLER_H
#define DUMMY_INPUT_CONTROLLER_H

#include "IDummyController.h"

class DummyInputController : public IDummyController {
	public:
		DummyInputController(Configuration *conf);
		virtual ~DummyInputController();

		virtual void sync(SyncDirection direction);
};

#endif /* DUMMY_INPUT_CONTROLLER_H */
