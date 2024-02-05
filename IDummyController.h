#ifndef IDUMMY_CONTROLLER_H
#define IDUMMY_CONTROLLER_H

#include "IController.h"
#include "Configuration.h"

class IDummyController : public IController {
	public:
		IDummyController(ControllerType type);
		virtual ~IDummyController();

		virtual void sync(SyncDirection direction) = 0;
};

#endif /* IDUMMY_CONTROLLER_H */
