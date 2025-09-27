#ifndef DUMMY_OUTPUT_CONTROLLER_H
#define DUMMY_OUTPUT_CONTROLLER_H

#include "IDummyController.h"

class DummyOutputController : public IDummyController {
	private:
		Configuration *conf;

	public:
		DummyOutputController(Configuration *conf, bool dump_updates);
		virtual ~DummyOutputController();

		virtual void sync(SyncDirection direction);
};

#endif /* DUMMY_OUTPUT_CONTROLLER_H */
