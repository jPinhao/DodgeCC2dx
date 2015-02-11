#include "SpawnController.h"

SpawnController* SpawnController::create()
{
	SpawnController *ret = new(std::nothrow) SpawnController();
	if (ret && ret->init())
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

bool SpawnController::init()
{
	Vec2 windowOrigin = Director::getInstance()->getVisibleOrigin();
	Size windowSize = Director::getInstance()->getVisibleSize();
	setContentSize(windowSize);
	//setPosition((windowOrigin + windowSize) / 2);
	return true;
}

void SpawnController::onEnter()
{
	super::onEnter();
	//start spawning
	if (spawningEnabled && !allSpawners.empty()) startSpawning();
}

void SpawnController::update(float deltaTime)
{
	//double check we're allowed to spawn
	if (spawningEnabled && !allSpawners.empty())
	{
		nextSpawn -= deltaTime;
		if (nextSpawn <= 0)
		{
			nextSpawn = spawnRate;
			
			short randomSpawner = RandomHelper::random_int(0,(int)allSpawners.size()-1);
			SpawnVolume *spawner = allSpawners[randomSpawner];
			if (spawner) spawner->spawnUnit();
		}
	}
}

void SpawnController::registerSpawner(SpawnVolume* spawner)
{
	if (!spawner) return;
	//check if this spawner has already been registered
	if (std::find(allSpawners.begin(), allSpawners.end(), spawner) != allSpawners.end())
	{
		return;
	}

	allSpawners.push_back(spawner);
	spawner->retain(); 
	//start spawning, we've got someone to manage!
	if (spawningEnabled && allSpawners.size() == 1) startSpawning();
}
void SpawnController::unregisterSpawner(SpawnVolume* spawner)
{
	if (!spawner) return;
	std::vector<SpawnVolume*>::iterator new_end = std::remove(allSpawners.begin(), allSpawners.end(), spawner);
	//double check we do contain the spawner so we don't release a reference we don't hold
	if (new_end != allSpawners.end())
	{
		allSpawners.erase(new_end, allSpawners.end());
		spawner->release();
	}
	//disable spawning, we're not managing anyone
	if (allSpawners.empty()) stopSpawning();
}

void SpawnController::enableSpawning(bool enable)
{
	if (spawningEnabled != enable)
	{
		spawningEnabled = enable;
		if (isRunning())
		{
			if (spawningEnabled)
			{
				if (!allSpawners.empty()) startSpawning();
			}
			else stopSpawning();
		}
	}
}

void SpawnController::startSpawning()
{
	nextSpawn = INITIAL_SPAWNDELAY;
	scheduleUpdate();
}

void SpawnController::stopSpawning()
{
	unscheduleUpdate();
}



SpawnController::SpawnController()
	: super()
	, spawnRate(DEFAULT_SPAWNRATE)
	, spawningEnabled(true)
{
}

SpawnController::~SpawnController()
{
	if (!allSpawners.empty())
	{
		for (auto spawner : allSpawners)
		{
			spawner->release();
		}
		allSpawners.clear();
	}
}