#include "SpawnController.h"

SpawnController* SpawnController::create(MultiControllerUpdate controllerUpdateType/* = MultiControllerUpdate::RAND_SELECTOR*/)
{
	SpawnController *ret = new(std::nothrow) SpawnController();
	if (ret && ret->init(controllerUpdateType))
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

void SpawnController::update(float deltaTime)
{
	if (enabled && isControlling())
	{
		nextSpawn -= deltaTime;
		if (nextSpawn <= 0)
		{
			nextSpawn = spawnRate;
			super::update(deltaTime);
		}
	}
	else stopController();
}

void SpawnController::startController()
{
	nextSpawn = INITIAL_SPAWNDELAY;
	super::startController();
}


SpawnController::SpawnController()
	: super()
	, spawnRate(DEFAULT_SPAWNRATE)
{
}