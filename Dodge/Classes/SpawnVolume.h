#ifndef __SPAWNVOLUME_H__
#define __SPAWNVOLUME_H__

#include "cocos2d.h"
#include "Pellet.h"
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

	enum VolumeCollision
	{
		COLLISION_NONE,
		COLLISION_SPRITE,
		COLLISION_VOLUME
	};

	static SpawnVolume* create(const Pellet& unitsToSpawn,VolumeCollision collisionType = VolumeCollision::COLLISION_NONE);
	static SpawnVolume* createWithSprite(const Pellet& unitsToSpawn,cocos2d::Sprite *sprite, VolumeCollision collisionType = VolumeCollision::COLLISION_SPRITE);

	void setVolumeSprite(cocos2d::Sprite *sprite);
	//set the spawning direction -> if we have a sprite this is will take the prite's rotation into account
	void setSpawnDirection(const cocos2d::Vec2& direction);

	virtual void onEnter() override;
	virtual void onExit() override;

	void controlledUpdate() override;
	void spawnUnit();

CC_CONSTRUCTOR_ACCESS:
	SpawnVolume();
	~SpawnVolume();
	bool initWithSprite(const Pellet& unitsToSpawn, cocos2d::Sprite *sprite, VolumeCollision collisionType);

private:
	typedef Pawn super;

	void resizeToFit();
	void updatePhysicsBodyShape();
	bool FindSpawnAreaEdge(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data);

	void registerWithController();
	void unregisterWithController();

	cocos2d::Sprite* volumeSprite;
	cocos2d::Vec2 volumeCentre;
	//these should be set in the order { BL,TL,TR,BR }
	cocos2d::Vec2 collisionVertices[4];
	VolumeCollision myCollisionType;
	
	//pointer to our controller, so we can remove ourselves
	SpawnController *myController;

	//Spawning attributes
	Pellet *spawnUnitType;
	//this is the direction from which new entities are spawned (relative to sprite (or volume). If 0,0 then we spawn from anywhere in SpawnArea)
	cocos2d::Vec2 spawnDirection;
	//centre point of the SpawnArea
	cocos2d::Vec2 spawnAreaCentre;
	//Extent of the SpawnArea; centre +/- extent = spawn area
	cocos2d::Vec2 spawnAreaExtentForward;
	cocos2d::Vec2 spawnAreaExtentUp;
};

#endif