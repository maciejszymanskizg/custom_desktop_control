#include <stdlib.h>
#include "DummyInputController.h"
#include "TrainConfigurationIDs.h"

#define RANDOM_CONDITION(x_) (rand() % x_ == 0)
#define RANDOM_VALUE(x_) (rand() % x_)

DummyInputController::DummyInputController(Configuration *conf) : IDummyController(ControllerType::HOST_CONTROLLER)
{
	this->conf = conf;
}

DummyInputController::~DummyInputController()
{

}

void DummyInputController::generateRandomData(void)
{
	this->conf->accessLock();

	if (RANDOM_CONDITION(5)) {
		this->conf->setValue(CONFIGURATION_ID_HASLER_VELOCITY, RANDOM_VALUE(150));
	}

	if (RANDOM_CONDITION(4)) {
		this->conf->setValue(CONFIGURATION_ID_VOLTMETER_LOW_VOLTAGE, RANDOM_VALUE(150));
	}

	this->conf->accessUnlock();
}

void DummyInputController::sync(IController::SyncDirection dir)
{
	generateRandomData();
}
