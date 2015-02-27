#ifndef __SPAWNCONTROLLER_H__
#define __SPAWNCONTROLLER_H__

#include "cocos2d.h"
#include "CoreEntities\MultiController.h"
#include "SpawnVolume.h"

#define DEFAULT_SPAWNRATE 2.f
#define INITIAL_SPAWNDELAY .5f

//forward declaration
class SpawnVolume;

/*
Controller for spawners
SpawnController manages a group of spawners and decides when to spawn the next entity, and which spawner should be providing it
*/
class SpawnController : public MultiController
{
public:
	static SpawnController* create(MultiControllerUpdate controllerUpdateType = MultiControllerUpdate::RAND_SELECTOR);

	//update() ticks down our spawn timer and decides whether it's time to spawn a new pellet
	void update(float deltaTime) override;
	//start controller and reset the nextSpawn time to INITIAL_SPAWNDELAY
	void startController() override;

CC_CONSTRUCTOR_ACCESS:
	SpawnController();

private:
	typedef MultiController super;

	float spawnRate;
	float nextSpawn;
};

#endif