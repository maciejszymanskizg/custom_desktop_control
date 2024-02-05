#include "DummyOutputController.h"

DummyOutputController::DummyOutputController(Configuration *conf) : IDummyController(ControllerType::PHYS_CONTROLLER)
{
}

DummyOutputController::~DummyOutputController()
{

}

void DummyOutputController::sync(IController::SyncDirection dir)
{

}
