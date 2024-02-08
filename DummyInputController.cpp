#include "DummyInputController.h"

DummyInputController::DummyInputController(Configuration *conf) : IDummyController(ControllerType::HOST_CONTROLLER)
{
	this->conf = conf;
}

DummyInputController::~DummyInputController()
{

}

void DummyInputController::sync(IController::SyncDirection dir)
{
	conf->accessLock();
	conf->dumpConfigUpdates();
	conf->accessUnlock();
}
