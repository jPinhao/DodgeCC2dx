#ifndef __PELLETSPAWNER_H__
#define __PELLETSPAWNER_H__

#include "cocos2d.h"
#include "Pellet.h"

USING_NS_CC;

// Node that can spawn new Entities
template <class spawnUnitType>
class SpawnVolume : public cocos2d::Node
{
public:
	enum VolumeCollision
	{
		COLLISION_NONE,
		COLLISION_SPRITE,
		COLLISION_VOLUME
	};

	static SpawnVolume* create(VolumeCollision collisionType = VolumeCollision::COLLISION_NONE);
	static SpawnVolume* createWithSprite(cocos2d::Sprite *sprite, VolumeCollision collisionType = VolumeCollision::COLLISION_SPRITE);

	void setVolumeSprite(cocos2d::Sprite *sprite);
	//set the spawning direction -> if we have a sprite this is will take the prite's rotation into account
	void setSpawnDirection(const cocos2d::Vec2& direction);

	//virtual void onEnter() override;
	//virtual void onExit() override;

	//void spawn();

CC_CONSTRUCTOR_ACCESS:
	SpawnVolume();
	bool initWithSprite(cocos2d::Sprite *sprite, VolumeCollision collisionType);

private:
	typedef cocos2d::Node super;

	void resizeToFitSprite();
	void updatePhysicsBodyShape();

	cocos2d::Sprite* volumeSprite;
	cocos2d::Vec2 collisionVertices[4];
	VolumeCollision myCollisionType;
	cocos2d::Vec2 spawnDirection;
};


//function definitions
template<class spawnUnitType>
SpawnVolume<spawnUnitType>::SpawnVolume()
	:super(),
	volumeSprite(nullptr)
{

}

template <class spawnUnitType>
SpawnVolume<spawnUnitType>* SpawnVolume<spawnUnitType>::create(VolumeCollision collisionType = VolumeCollision::COLLISION_NONE)
{
	SpawnVolume<spawnUnitType> *ret = new (std::nothrow) SpawnVolume<spawnUnitType>();
	if (ret != nullptr && ret->initWithSprite(nullptr, collisionType))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

template <class spawnUnitType>
SpawnVolume<spawnUnitType>* SpawnVolume<spawnUnitType>::createWithSprite(Sprite *sprite, VolumeCollision collisionType = VolumeCollision::COLLISION_SPRITE)
{
	SpawnVolume<spawnUnitType> *ret = new (std::nothrow) SpawnVolume<spawnUnitType>();
	if (ret != nullptr && ret->initWithSprite(sprite, collisionType))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

template <class spawnUnitType>
bool SpawnVolume<spawnUnitType>::initWithSprite(cocos2d::Sprite *sprite, VolumeCollision collisionType)
{
	CCASSERT((sprite != nullptr || collisionType != VolumeCollision::COLLISION_SPRITE), "ERROR: Tried to create a spawn volume without a sprite but collision=sprite- setting to no collision");
	volumeSprite = sprite;
	myCollisionType = collisionType;
	if (volumeSprite)
	{
		addChild(volumeSprite);
	}
	else if (myCollisionType == VolumeCollision::COLLISION_SPRITE)
	{
		myCollisionType = VolumeCollision::COLLISION_NONE;
	}

	if (myCollisionType != VolumeCollision::COLLISION_NONE)
	{
		PhysicsBody *spawnVolumeBody = PhysicsBody::create();
		spawnVolumeBody->setEnable(true);
		spawnVolumeBody->setDynamic(false);
		spawnVolumeBody->setContactTestBitmask(0xFFFFFFFF);
		setPhysicsBody(spawnVolumeBody);
	}

	resizeToFitSprite();
	return true;
}

template <class spawnUnitType>
void SpawnVolume<spawnUnitType>::resizeToFitSprite()
{
	if (volumeSprite)
	{
		Size spriteSize = Size(volumeSprite->getContentSize().width*volumeSprite->getScaleX(), volumeSprite->getContentSize().height*volumeSprite->getScaleY());
		Vec2 spriteCentre = Vec2(spriteSize.width, spriteSize.height) / 2;
		//cocos angles are clockwise.... so need to invert for actual angle fo correct cos/sin
		float spriteRotation = CC_DEGREES_TO_RADIANS(-volumeSprite->getRotationSkewX());

		collisionVertices[0] = Vec2(0.f, 0.f);
		collisionVertices[1] = Vec2(0.f, spriteSize.height);
		collisionVertices[2] = Vec2(spriteSize.width, spriteSize.height);
		collisionVertices[3] = Vec2(spriteSize.width, 0.f);

		//rotate the size dimensions so we fit the sprite
		Size volumeSize; 
		float absCos = abs(cosf(spriteRotation)), absSin = abs(sinf(spriteRotation));
		volumeSize.width = spriteSize.width*absCos + spriteSize.height*absSin;
		volumeSize.height = spriteSize.height*absCos + spriteSize.width*absSin;
		//center the sprite
		volumeSprite->setAnchorPoint(Vec2(.5f, .5f));
		volumeSprite->setPosition(.5f * volumeSize.width, .5f * volumeSize.height);
		//update the volume settings to fit the sprite
		setContentSize(volumeSize);

		//calculate the new collision vertices based on the rotation (used in Physics shape creation)
		//we don't translate them using the volume centre as the physics shapes are positioned relative to the centre
		Vec2 rotateV;
		for (auto& vertex : collisionVertices)
		{
			rotateV = vertex - spriteCentre;
			vertex.x = rotateV.x*cosf(spriteRotation) - rotateV.y*sinf(spriteRotation);
			vertex.y = rotateV.x*sinf(spriteRotation) + rotateV.y*cosf(spriteRotation);
		}
	}
	updatePhysicsBodyShape();
}

template <class spawnUnitType>
void SpawnVolume<spawnUnitType>::updatePhysicsBodyShape()
{
	PhysicsBody *myBody = getPhysicsBody();
	if (myBody)
	{
		myBody->removeAllShapes();
		if (myCollisionType == VolumeCollision::COLLISION_VOLUME)
		{
			myBody->addShape(PhysicsShapeBox::create(Size(getContentSize().width, getContentSize().height),
				PhysicsMaterial(0.f, 1.f, 0.f)));
		}
		else if (myCollisionType == VolumeCollision::COLLISION_SPRITE)
		{
			PhysicsShapePolygon *colPolygon = PhysicsShapePolygon::create(&collisionVertices[0], 4,
				PhysicsMaterial(0.f, 1.f, 0.f));
			colPolygon->setContactTestBitmask(0xFFFFFFFF);
			myBody->addShape(colPolygon, false);
		}
	}
}
#endif