#ifndef __SPAWNVOLUME_H__
#define __SPAWNVOLUME_H__

#include "cocos2d.h"
#include "CoreEntities\Pawn.h"
#include "SpawnController.h"
#include "LevelManager.h"

USING_NS_CC;

#define DEFAULT_SPAWNDIR Vec2(0.f,-1.f)

//forward declaration
class SpawnController;

// Node that can spawn new Entities
class SpawnVolume : public Pawn
{
public:
	static SpawnVolume* create(const Pawn& unitsToSpawn, SpawnController *controller);
	static SpawnVolume* createWithSprite(const Pawn& unitsToSpawn, cocos2d::Sprite *sprite, SpawnController *controller);

	//clones an instance of the SpawnVolume and returns it's pointer
	//*a Controller must be attached manually
	SpawnVolume* clone() const;

	virtual void onEnter() override;

	void controlledUpdate() override;
	void spawnUnit();

CC_CONSTRUCTOR_ACCESS:
	SpawnVolume();
	~SpawnVolume();
	bool initWithSprite(const Pawn& unitsToSpawn, cocos2d::Sprite *sprite, SpawnController *controller);

	//used by clone() to copy the members
	SpawnVolume(const SpawnVolume& copy);
	void operator=(const SpawnVolume& copy) = delete;

	void setupPhysicsBody() override;

private:
	typedef Pawn super;

	bool FindSpawnAreaEdge(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data);

	void registerWithController();
	void unregisterWithController();

	//Spawning attributes
	Pawn *spawnUnitType;
	//this is the direction from which new entities are spawned (relative to sprite (or volume). If 0,0 then we spawn from anywhere in SpawnArea)
	cocos2d::Vec2 spawnDirection;
	//centre point of the SpawnArea
	cocos2d::Vec2 spawnAreaCentre;
	//Extent of the SpawnArea; centre +/- extent = spawn area
	cocos2d::Vec2 spawnAreaExtentForward;
	cocos2d::Vec2 spawnAreaExtentUp;
};

#endif