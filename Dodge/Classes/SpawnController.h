#ifndef __SPAWNCONTROLLER_H__
#define __SPAWNCONTROLLER_H__

#include "cocos2d.h"
#include "SpawnVolume.h"

#define DEFAULT_SPAWNRATE 2.f
#define INITIAL_SPAWNDELAY .5f

//forward declaration
class SpawnVolume;

class SpawnController : public cocos2d::Node
{
public:
	static SpawnController* create();

	//try to start spawning (if we're enabled and have registered spawners)
	virtual void onEnter() override;
	//update() ticks down our spawn timer and decides whether it's time to spawn
	virtual void update(float deltaTime) override;

	//add a new spawner to this manager
	void registerSpawner(SpawnVolume* spawner);
	//remove a spawner from this manager
	void unregisterSpawner(SpawnVolume* spawner);
	//enable spawning, this will start spawning if we're managing any spawners
	void enableSpawning(bool enable);

CC_CONSTRUCTOR_ACCESS:
	bool init();
	SpawnController();
	~SpawnController();

private:
	typedef cocos2d::Node super;

	//start the spawning updates
	void startSpawning();
	void stopSpawning();

	std::vector<SpawnVolume*> allSpawners;
	float spawnRate;
	float nextSpawn;
	bool spawningEnabled;
};

#endif