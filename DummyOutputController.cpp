#include "DummyOutputController.h"

DummyOutputController::DummyOutputController(Configuration *conf) : IDummyController(ControllerType::PHYS_CONTROLLER)
{
	this->conf = conf;
}

DummyOutputController::~DummyOutputController()
{

}

void DummyOutputController::sync(IController::SyncDirection dir)
{
	conf->accessLock();
	conf->dumpConfigUpdates();
	conf->accessUnlock();
}
