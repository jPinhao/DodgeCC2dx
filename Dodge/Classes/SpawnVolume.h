#ifndef __PELLETSPAWNER_H__
#define __PELLETSPAWNER_H__

#include "cocos2d.h"
#include "Pellet.h"

USING_NS_CC;

#define DEFAULT_SPAWNDIR Vec2(0.f,-1.f)

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

	virtual void onEnter() override;
	//virtual void onExit() override;

	//void spawn();

CC_CONSTRUCTOR_ACCESS:
	SpawnVolume();
	bool initWithSprite(cocos2d::Sprite *sprite, VolumeCollision collisionType);

private:
	typedef cocos2d::Node super;

	void resizeToFit();
	void updatePhysicsBodyShape();
	bool FindSpawnAreaEdge(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data);

	cocos2d::Sprite* volumeSprite;
	//these should be set in the order { BL,TL,TR,BR }
	cocos2d::Vec2 collisionVertices[4];
	VolumeCollision myCollisionType;
	//this is the direction from which new entities are spawned (relative to sprite (or volume). If 0,0 then we spawn from anywhere in SpawnArea)
	cocos2d::Vec2 spawnDirection;
	//centre point of the SpawnArea
	cocos2d::Vec2 spawnAreaCentre;
	//Extent of the SpawnArea; centre +/- extent = spawn area
	cocos2d::Vec2 spawnAreaExtentForward;
	cocos2d::Vec2 spawnAreaExtentUp;
};


//function definitions
template<class spawnUnitType>
SpawnVolume<spawnUnitType>::SpawnVolume()
	:super(),
	volumeSprite(nullptr),
	spawnDirection(DEFAULT_SPAWNDIR)
{

}

template <class spawnUnitType>
SpawnVolume<spawnUnitType>* SpawnVolume<spawnUnitType>::create(VolumeCollision collisionType/* = VolumeCollision::COLLISION_NONE*/)
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
SpawnVolume<spawnUnitType>* SpawnVolume<spawnUnitType>::createWithSprite(Sprite *sprite, VolumeCollision collisionType/* = VolumeCollision::COLLISION_SPRITE*/)
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
	CCASSERT((sprite != nullptr || collisionType != COLLISION_SPRITE), "ERROR: Tried to create a spawn volume without a sprite but collision=sprite- setting to no collision");
	volumeSprite = sprite;
	myCollisionType = collisionType;

	PhysicsBody *spawnVolumeBody = PhysicsBody::create();
	spawnVolumeBody->setDynamic(false);
	spawnVolumeBody->setContactTestBitmask(0xFFFFFFFF);
	setPhysicsBody(spawnVolumeBody);

	if (volumeSprite)
	{
		addChild(volumeSprite);
	}
	else if (myCollisionType == COLLISION_SPRITE)
	{
		myCollisionType = COLLISION_NONE;
	}

	if (myCollisionType == COLLISION_NONE) spawnVolumeBody->setEnable(false);

	resizeToFit();
	return true;
}

template <class spawnUnitType>
void SpawnVolume<spawnUnitType>::resizeToFit()
{
	if (volumeSprite)
	{
		Size spriteSize = Size(volumeSprite->getContentSize().width*volumeSprite->getScaleX(), volumeSprite->getContentSize().height*volumeSprite->getScaleY());
		Vec2 spriteCentre = Vec2(spriteSize.width, spriteSize.height) / 2;
		//cocos angles are clockwise.... so need to invert for actual angle fo correct cos/sin
		float spriteRotation = CC_DEGREES_TO_RADIANS(-volumeSprite->getRotation());

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

		if (myCollisionType == COLLISION_VOLUME)
		{
			Size halfSize = volumeSize/2;
			collisionVertices[0] = Vec2(-halfSize.width, -halfSize.height);
			collisionVertices[1] = Vec2(-halfSize.width, halfSize.height);
			collisionVertices[2] = Vec2(halfSize.width, halfSize.height);
			collisionVertices[3] = Vec2(halfSize.width, -halfSize.height);
		}
		else
		{
			collisionVertices[0] = Vec2(0.f, 0.f);
			collisionVertices[1] = Vec2(0.f, spriteSize.height);
			collisionVertices[2] = Vec2(spriteSize.width, spriteSize.height);
			collisionVertices[3] = Vec2(spriteSize.width, 0.f);
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
	}
	else
	{
		Size halfSize = getContentSize()/2;
		collisionVertices[0] = Vec2(-halfSize.width, -halfSize.height);
		collisionVertices[1] = Vec2(-halfSize.width, halfSize.height);
		collisionVertices[2] = Vec2(halfSize.width, halfSize.height);
		collisionVertices[3] = Vec2(halfSize.width, -halfSize.height);
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
		PhysicsShapePolygon *colPolygon = PhysicsShapePolygon::create(&collisionVertices[0], 4,
			PhysicsMaterial(0.f, 1.f, 0.f));
		colPolygon->setContactTestBitmask(0xFFFFFFFF);
		myBody->addShape(colPolygon, false);
	}
}

//setup the valid area from which to spawn new units
template <class spawnUnitType>
void SpawnVolume<spawnUnitType>::onEnter()
{
	super::onEnter();
	//resize spawnArea to fit within other PelletSpawners and force spawning inside the box
	Vec2 worldCentre, extent, extentUp, rotatedExtent, rotatedExtentUp;

	//get the current component centre and extent
	CCASSERT(getAnchorPoint() == Vec2(.5f, .5f), "ERROR: SpawnComponent anchor needs to be ste to .5,.5 for it's maths and physics to work correctly");
	worldCentre = getPosition();
	if (getPhysicsBody() && getPhysicsBody()->getFirstShape())
	{
		worldCentre += getPhysicsBody()->getFirstShape()->getCenter();
		//deduce the extent and extentUp from the collisionVertices (order= { BL,TL,TR,BR });
		extent = (collisionVertices[0] + collisionVertices[1]) / 2 - getPhysicsBody()->getFirstShape()->getCenter();
		extentUp = (collisionVertices[1] + collisionVertices[2]) / 2 - getPhysicsBody()->getFirstShape()->getCenter();
		//transform the extent to get a vector with the current component orientation (keep only X component, that's the only relevant scaled axis)
		//bear in mind it is already rotate with regards to the physics shape, so we just need to apply the SpawnVolume rotation
		float myRotation = CC_DEGREES_TO_RADIANS(-getRotation());
		rotatedExtent = Vec2(extent.x * cosf(myRotation) - extent.y*sinf(myRotation),
			extent.x*sinf(myRotation) + extent.y*cosf(myRotation));
		rotatedExtentUp = Vec2(extentUp.x * cosf(myRotation) - extentUp.y*sinf(myRotation),
			extentUp.x*sinf(myRotation) + extentUp.y*cosf(myRotation));
	}
	//check for colisions between spawn component centre and extremeties
	PhysicsWorld *physWorld = getPhysicsBody()->getWorld();
	//cast 4 rays, 2 along each direction, to get the minimum spawn area (in case of angled collisions)
	Vec2 posEdge1 = worldCentre + rotatedExtent + rotatedExtentUp, posEdge2 = worldCentre + rotatedExtent - rotatedExtentUp,
		negEdge1 = worldCentre - rotatedExtent + rotatedExtentUp, negEdge2 = worldCentre - rotatedExtent - rotatedExtentUp;
	//FindSpawnAreaEdge will store the shortest distance collision into the &xEdgex Vector
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume<spawnUnitType>::FindSpawnAreaEdge, this), worldCentre + rotatedExtentUp, posEdge1, static_cast<void*>(&posEdge1));
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume<spawnUnitType>::FindSpawnAreaEdge, this), worldCentre - rotatedExtentUp, posEdge2, static_cast<void*>(&posEdge2));
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume<spawnUnitType>::FindSpawnAreaEdge, this), worldCentre + rotatedExtentUp, negEdge1, static_cast<void*>(&negEdge1));
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume<spawnUnitType>::FindSpawnAreaEdge, this), worldCentre - rotatedExtentUp, negEdge2, static_cast<void*>(&negEdge2));

	//get the smallest forward and backwards hits and recentre the,
	if ((worldCentre - posEdge2).length() < (worldCentre - posEdge1).length())
	{
		posEdge1 = posEdge2 + rotatedExtentUp;
	}
	else posEdge1 -= rotatedExtentUp;

	if ((worldCentre - negEdge2).length() < (worldCentre - negEdge1).length())
	{
		negEdge1 = negEdge2 + rotatedExtentUp;
	}
	else negEdge1 -= rotatedExtentUp;

	//desired new extent and component centre
	spawnAreaExtentForward = (posEdge1 - negEdge1) / 2;
	spawnAreaExtentUp = rotatedExtentUp;
	spawnAreaCentre = (posEdge1 - spawnAreaExtentForward) - worldCentre + getContentSize() / 2;
	
	////register ourselves into the spawncontroller
	//if (bCanEverSpawn)
	//{
	//	ADodgeGameMode *gameMode = Cast<ADodgeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	//	if (gameMode)
	//	{
	//		ASpawnController *controller = gameMode->GetSpawnController();
	//		controller->RegisterSpawner(this);
	//	}
	//}
	
	
	//////////////////////////////////////////////////
	////// TEST DRAW CODE
	rotatedExtent = 2 * spawnAreaExtentForward;
	rotatedExtent.x = abs(rotatedExtent.x);
	rotatedExtent.y = abs(rotatedExtent.y);
	float myRotation = CC_DEGREES_TO_RADIANS(volumeSprite->getRotation());
	rotatedExtent = Vec2(rotatedExtent.x * abs(cosf(myRotation)) + rotatedExtent.y * abs(sinf(myRotation)),
	rotatedExtent.x * abs(sinf(myRotation)) - rotatedExtent.y * abs(cosf(myRotation)));
	Size testSpriteSize = Size(rotatedExtent.x, volumeSprite->getContentSize().height*volumeSprite->getScaleY());
	Vec2 testSpriteOrigin = Vec2(spawnAreaCentre.x - testSpriteSize.width / 2, spawnAreaCentre.y - testSpriteSize.height / 2);
	auto testSprite = Sprite::create("debug_tex.png",
	Rect(0, 0, testSpriteSize.width, testSpriteSize.height));
	Texture2D::TexParams params = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	testSprite->getTexture()->setTexParameters(params);
	testSprite->setAnchorPoint(Vec2(.5f, .5f));
	testSprite->setPosition(spawnAreaCentre);
	testSprite->setRotation(volumeSprite->getRotation());
	addChild(testSprite);
	////// TEST DRAW CODE
	//////////////////////////////////////////////////
}

template <class spawnUnitType>
bool SpawnVolume<spawnUnitType>::FindSpawnAreaEdge(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data)
{
	if (getPhysicsBody() != info.shape->getBody())
	{
		Vec2 *hitEndReturn = static_cast<Vec2*>(data);
		if (data)
		{
			//if the new contact is closer than the previous registered one save it
			if ((info.start - info.contact).length() < (info.start - *hitEndReturn).length())
			{
				*hitEndReturn = info.contact;
			}
		}
	}
	return true;
}

#endif