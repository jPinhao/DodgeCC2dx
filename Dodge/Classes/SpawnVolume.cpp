#include "SpawnVolume.h"
#include "SpawnController.h"

SpawnVolume* SpawnVolume::create(const Pawn& unitsToSpawn, SpawnController *controller)
{
	SpawnVolume *ret = new (std::nothrow) SpawnVolume();
	if (ret != nullptr && ret->initWithSprite(unitsToSpawn, nullptr, controller))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

SpawnVolume* SpawnVolume::createWithSprite(const Pawn& unitsToSpawn, Sprite *sprite, SpawnController *controller)
{
	SpawnVolume *ret = new (std::nothrow) SpawnVolume();
	if (ret != nullptr && ret->initWithSprite(unitsToSpawn, sprite, controller))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

bool SpawnVolume::initWithSprite(const Pawn& unitsToSpawn, Sprite *sprite, SpawnController *controller)
{
	spawnUnitType = unitsToSpawn.clone();
	spawnUnitType->retain();

	if (sprite)
	{
		PhysicsBody *spawnVolumeBody = PhysicsBody::create();
		spawnVolumeBody->setDynamic(false);
		spawnVolumeBody->setContactTestBitmask(0xFFFFFFFF);
		setPhysicsBody(spawnVolumeBody);
	}

	return super::initWithController(controller, sprite);
}

void SpawnVolume::setupPhysicsBody()
{
	super::setupPhysicsBody();
	if (getPhysicsBody())
	{
		getPhysicsBody()->setDynamic(false);
		getPhysicsBody()->setMass(PHYSICS_INFINITY);
		getPhysicsBody()->setMoment(PHYSICS_INFINITY);
	}
}

//setup the valid area from which to spawn new units
void SpawnVolume::onEnter()
{
	super::onEnter();

	//resize spawnArea to fit within other PelletSpawners and force spawning inside the box
	Vec2 worldCentre, extent, extentUp, rotatedExtent, rotatedExtentUp;
	//these 2 are used later to mody the spawn area so the spawned objects don't overlap spawner/blocking objects
	//add a couple of units to actual extent so a collision doesn't get immedeately triggeres when spawned
	Vec2 spawnUnitExtent = Vec2(spawnUnitType->getContentSize().width*spawnUnitType->getScaleX() +2.f, 0.f) / 2;
	Vec2 spawnUnitExtentUp = Vec2(0.f, spawnUnitType->getContentSize().height*spawnUnitType->getScaleY() + 2.f) / 2;
	spawnUnitExtent = spawnUnitExtent.rotateByAngle(Vec2::ZERO, CC_DEGREES_TO_RADIANS(-mySprite->getRotation()));
	spawnUnitExtentUp = spawnUnitExtentUp.rotateByAngle(Vec2::ZERO, CC_DEGREES_TO_RADIANS(-mySprite->getRotation()));

	//get the current component centre and extent
	CCASSERT(getAnchorPoint() == Vec2(.5f, .5f), "ERROR: SpawnComponent anchor needs to be ste to .5,.5 for it's maths and physics to work correctly");
	worldCentre = getPosition();
	if (getPhysicsBody() && getPhysicsBody()->getFirstShape())
	{
		worldCentre += getPhysicsBody()->getFirstShape()->getCenter();
		//deduce the extent and extentUp from the collisionVertices (order= { BL,TL,TR,BR });
		extent = (spriteVertices[0] + spriteVertices[1]) / 2 - getPhysicsBody()->getFirstShape()->getCenter();
		extentUp = (spriteVertices[1] + spriteVertices[2]) / 2 - getPhysicsBody()->getFirstShape()->getCenter();
		//transform the extent to get a vector with the current component orientation (keep only X component, that's the only relevant scaled axis)
		//bear in mind it is already rotate with regards to the physics shape, so we just need to apply the SpawnVolume rotation
		float myRotation = CC_DEGREES_TO_RADIANS(-getRotation());
		rotatedExtent = extent.rotateByAngle(Vec2(0.f, 0.f), myRotation);
		rotatedExtentUp = extentUp.rotateByAngle(Vec2(0.f, 0.f), myRotation);
		spawnUnitExtent = spawnUnitExtent.rotateByAngle(Vec2::ZERO, myRotation);
		spawnUnitExtentUp = spawnUnitExtentUp.rotateByAngle(Vec2::ZERO, myRotation);
		/*rotatedExtent = Vec2(extent.x * cosf(myRotation) - extent.y*sinf(myRotation),
			extent.x*sinf(myRotation) + extent.y*cosf(myRotation));
		rotatedExtentUp = Vec2(extentUp.x * cosf(myRotation) - extentUp.y*sinf(myRotation),
			extentUp.x*sinf(myRotation) + extentUp.y*cosf(myRotation));*/
	}
	//check for colisions between spawn component centre and extremeties
	PhysicsWorld *physWorld = getPhysicsBody()->getWorld();
	//cast 4 rays, 2 along each direction, to get the minimum spawn area (in case of angled collisions)
	Vec2 posEdge1 = worldCentre + rotatedExtent + rotatedExtentUp, posEdge2 = worldCentre + rotatedExtent - rotatedExtentUp,
		negEdge1 = worldCentre - rotatedExtent + rotatedExtentUp, negEdge2 = worldCentre - rotatedExtent - rotatedExtentUp;
	//FindSpawnAreaEdge will store the shortest distance collision into the &xEdgex Vector
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume::FindSpawnAreaEdge, this), worldCentre + rotatedExtentUp, posEdge1, static_cast<void*>(&posEdge1));
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume::FindSpawnAreaEdge, this), worldCentre - rotatedExtentUp, posEdge2, static_cast<void*>(&posEdge2));
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume::FindSpawnAreaEdge, this), worldCentre + rotatedExtentUp, negEdge1, static_cast<void*>(&negEdge1));
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume::FindSpawnAreaEdge, this), worldCentre - rotatedExtentUp, negEdge2, static_cast<void*>(&negEdge2));

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
	negEdge1 -= spawnUnitExtent;
	posEdge1 += spawnUnitExtent;
	spawnAreaExtentForward = (negEdge1 - posEdge1) / 2;
	spawnAreaCentre = (negEdge1 - spawnAreaExtentForward) - worldCentre + getContentSize() / 2;
	spawnAreaExtentUp = rotatedExtentUp + spawnUnitExtentUp;


	//////////////////////////////////////////////////
	////// TEST DRAW CODE
	//LevelManager::getInstance()->getCurrentLevel()->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	//rotatedExtent = 2 * spawnAreaExtentForward;
	//rotatedExtentUp = 2 * spawnAreaExtentUp;
	///*rotatedExtent.x = abs(rotatedExtent.x);
	//rotatedExtent.y = abs(rotatedExtent.y);*/
	//float myRotation = CC_DEGREES_TO_RADIANS(mySprite->getRotation());
	//rotatedExtent = rotatedExtent.rotateByAngle(Vec2::ZERO, myRotation);
	//rotatedExtentUp = rotatedExtentUp.rotateByAngle(Vec2::ZERO, myRotation);
	//Size testSpriteSize = Size(rotatedExtent.x, rotatedExtentUp.y);
	//Vec2 testSpriteOrigin = Vec2(spawnAreaCentre.x - testSpriteSize.width / 2, spawnAreaCentre.y - testSpriteSize.height / 2);
	//auto testSprite = Sprite::create("debug_tex.png",
	//	Rect(0, 0, testSpriteSize.width, testSpriteSize.height));
	//Texture2D::TexParams params = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	//testSprite->getTexture()->setTexParameters(params);
	//testSprite->setAnchorPoint(Vec2(.5f, .5f));
	//testSprite->setPosition(spawnAreaCentre);
	//testSprite->setRotation(mySprite->getRotation());
	//addChild(testSprite);
	////// TEST DRAW CODE
	//////////////////////////////////////////////////
}

bool SpawnVolume::FindSpawnAreaEdge(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data)
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

void SpawnVolume::controlledUpdate()
{
	spawnUnit();
}

void SpawnVolume::spawnUnit()
{
	Vec2 spawnPoint, thrustVector;
	float spawnAngle;

	if (spawnDirection == Vec2::ZERO)
	{
		spawnPoint.x = RandomHelper::random_real<float>(-spawnAreaExtentUp.x, spawnAreaExtentUp.x);
		spawnPoint.y = RandomHelper::random_real<float>(-spawnAreaExtentUp.y, spawnAreaExtentUp.y);
		spawnAngle = RandomHelper::random_real<float>(0, 360);
	}
	else
	{
		//spawnAngle is relative to spawn axis, so constant for all, transformed later
		spawnAngle = RandomHelper::random_real<float>(-80, 80);
		if (spawnDirection.x == 0)
		{
			spawnPoint = spawnAreaExtentForward * rand_minus1_1() + spawnAreaExtentUp * spawnDirection.y;
		}
		else
		{
			spawnPoint = spawnAreaExtentUp * rand_minus1_1() + spawnAreaExtentForward * spawnDirection.y;
		}
	}

	//rotate by the spawner's rotation
	Vec2 volumeCentre = getContentSize()/2;
	spawnPoint.rotateByAngle(spawnAreaCentre, CC_DEGREES_TO_RADIANS(-getRotation()));
	//translate spawnPoint by spawnCentre and layer centre
	spawnPoint += getPosition() + (spawnAreaCentre - volumeCentre);
	//add spawner's rotation to our random angle
	spawnAngle -= getRotation() + mySprite->getRotation();
	thrustVector = spawnDirection.rotateByAngle(Vec2::ZERO, CC_DEGREES_TO_RADIANS(spawnAngle));
	float newAngle = thrustVector.getAngle();
	//and spawn!
	DodgeLevel *level = LevelManager::getInstance()->getCurrentLevel();
	Pawn *newUnit = level->spawnUnit(spawnUnitType->clone(), spawnPoint);
	if (newUnit)
	{
		//clones don't have a controller, we attach one manually
		newUnit->posessByAIController();
		newUnit->postInitializeCustom(&thrustVector);
	}
}

//function definitions
SpawnVolume::SpawnVolume()
	:super(),
	spawnDirection(DEFAULT_SPAWNDIR)
{

}

SpawnVolume::~SpawnVolume()
{
	spawnUnitType->release();
}

SpawnVolume* SpawnVolume::clone() const
{
	SpawnVolume* cloneVolume = new (std::nothrow) SpawnVolume(*this);
	if (cloneVolume && cloneVolume->initWithSprite(*spawnUnitType, cloneVolume->mySprite, nullptr))
	{
		cloneVolume->autorelease();
	}
	else CC_SAFE_DELETE(cloneVolume);
	return cloneVolume;
}

SpawnVolume::SpawnVolume(const SpawnVolume& copy)
	: super(copy)
{
	spawnUnitType = copy.spawnUnitType->clone();
	spawnDirection = copy.spawnDirection;
}