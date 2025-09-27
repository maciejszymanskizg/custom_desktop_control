#include <stdlib.h>
#include "DummyOutputController.h"
#include "TrainConfigurationIDs.h"

#define RANDOM_ID(max_, x_) ((rand() % (max_ + 1)) == x_)
#define RANDOM_VALUE(x_) (rand() % x_)


DummyOutputController::DummyOutputController(Configuration *conf) : IDummyController(ControllerType::PHYS_CONTROLLER)
{
	this->conf = conf;
}

DummyOutputController::~DummyOutputController()
{

}

void DummyOutputController::sync(IController::SyncDirection dir)
{
	conf->dumpConfigUpdates();
	conf->cleanUpdates();

	this->conf->accessLock();

	for (int i = 0; i <= CONFIGURATION_ID_INDICATOR_RADIO_SFN; i++)
	{
		if ((RANDOM_ID(CONFIGURATION_ID_INDICATOR_RADIO_SFN, i)) && (rand() % 2000 == 46))
		{
			this->conf->setValue(i, RANDOM_VALUE(this->conf->getMaxValue(i)));
		}
	}

	this->conf->accessUnlock();
}
