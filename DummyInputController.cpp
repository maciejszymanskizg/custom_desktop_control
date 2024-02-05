#include "DummyInputController.h"

DummyInputController::DummyInputController(Configuration *conf) : IDummyController(ControllerType::HOST_CONTROLLER)
{
}

DummyInputController::~DummyInputController()
{

}

void DummyInputController::sync(IController::SyncDirection dir)
{

}
