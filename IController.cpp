#include "IController.h"

IController::IController(IController::ControllerType type, bool dump_updates)
{
	this->type = type;
	this->dump_updates = dump_updates;
}

IController::~IController()
{

}

enum IController::ControllerType IController::getType(void)
{
	return this->type;
}

