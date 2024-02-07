#include "IController.h"

IController::IController(IController::ControllerType type)
{
	this->type = type;
}

IController::~IController()
{

}

enum IController::ControllerType IController::getType(void)
{
	return this->type;
}

