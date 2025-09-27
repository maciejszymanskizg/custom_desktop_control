#include <stdlib.h>
#include "DummyOutputController.h"
#include "TrainConfigurationIDs.h"

#define RANDOM_ID(max_, id_) ((rand() % max_) == id_)
#define RANDOM_VALUE(x_) (rand() % x_)

DummyOutputController::DummyOutputController(Configuration *conf, bool dump_updates) : IDummyController(ControllerType::PHYS_CONTROLLER, dump_updates)
{
	this->conf = conf;
}

DummyOutputController::~DummyOutputController()
{

}

void DummyOutputController::sync(IController::SyncDirection dir)
{
	if (dir == IController::SyncDirection::FROM_CONTROLLER) {
		this->conf->accessLock();

		for (int i = 0; i <= CONFIGURATION_ID_MAX; i++) {
			if ((RANDOM_ID(CONFIGURATION_ID_MAX, i)) && (rand() % 1000 == 726)) {
				this->conf->setValue(i, RANDOM_VALUE(this->conf->getMaxValue(i)));
			}
		}

		this->conf->accessUnlock();
	} else {
		if (this->dump_updates) {
			this->conf->accessLock();
			this->conf->dumpConfigUpdates();
			this->conf->cleanUpdates();
			this->conf->accessUnlock();
		}
	}
}
