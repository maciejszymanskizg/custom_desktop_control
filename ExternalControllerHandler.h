#ifndef EXTERNAL_CONTROLLER_HANDLER_H
#define EXTERNAL_CONTROLLER_HANDLER_H

#include <list>
#include "IController.h"

class ExternalControllerHandler {
	private:
		std::list<IController *> controllers;
	public:
		enum SyncDirection {
			SYNC_DIRECTION_FROM_CONTROLLER,
			SYNC_DIRECTION_TO_CONTROLLER
		};
		ExternalControllerHandler();
		~ExternalControllerHandler();
		bool addController(IController *c);
		void sync(IController::ControllerType type, IController::SyncDirection dir);
};

#endif /* EXTERNAL_CONTROLLER_HANDLER_H */
