#include "ExternalControllerHandler.h"

ExternalControllerHandler::ExternalControllerHandler()
{
}

ExternalControllerHandler::~ExternalControllerHandler()
{
}

bool ExternalControllerHandler::addController(IController *c)
{
	controllers.push_back(c);
	return true;
}

void ExternalControllerHandler::sync(IController::ControllerType type, IController::SyncDirection dir)
{
	std::list<IController *>::iterator it;

	for (it = controllers.begin(); it != controllers.end(); it++) {
		if ((*it)->getType() == type) {
			(*it)->sync(dir);
		}
	}
}
