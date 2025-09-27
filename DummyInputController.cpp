#include <stdlib.h>
#include "DummyInputController.h"
#include "TrainConfigurationIDs.h"

#define RANDOM_ID(max_, id_) ((rand() % max_) == id_)
#define RANDOM_VALUE(x_) (rand() % x_)

DummyInputController::DummyInputController(Configuration *conf, bool dump_updates) : IDummyController(ControllerType::HOST_CONTROLLER, dump_updates)
{
	this->conf = conf;
}

DummyInputController::~DummyInputController()
{

}

void DummyInputController::sync(IController::SyncDirection dir)
{
	if (dir == IController::SyncDirection::TO_CONTROLLER) {
		this->conf->accessLock();

		for (int i = 0; i < CONFIGURATION_ID_MAX; i++) {
			if ((RANDOM_ID(CONFIGURATION_ID_MAX, i)) && (rand() % 1000 == 278)) {
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
