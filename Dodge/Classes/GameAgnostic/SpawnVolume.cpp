#include "SpawnVolume.h"
#include "SpawnController.h"
#include "StaticHelpers.h"

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
	Vec2 worldCentre, volumeExtent, volumeExtentUp, spawnEdge;
	//get teh spawnvolume's effective transform (sprite->world if a sprite is linked)
	Mat4 volumeTransform;
	if(mySprite) volumeTransform = mySprite->getNodeToWorldTransform();
	else volumeTransform = getNodeToWorldTransform();
	//these 2 are used later to mody the spawn area so the spawned objects don't overlap spawner/blocking objects
	//add a couple of units to actual extent so a collision doesn't get immedeately triggeres when spawned
	Vec2 spawnUnitExtent = Vec2(spawnUnitType->getContentSize().width + 2.f, 0.f) / 2;
	MyHelpers::transformVec2(volumeTransform, &spawnUnitExtent);
	Vec2 spawnUnitExtentUp = Vec2(0.f, spawnUnitType->getContentSize().height + 2.f) / 2;
	MyHelpers::transformVec2(volumeTransform, &spawnUnitExtentUp);

	//get the current component centre and extent
	CCASSERT(getAnchorPoint() == Vec2(.5f, .5f), "ERROR: SpawnComponent anchor needs to be ste to .5,.5 for it's maths and physics to work correctly");
	//get the volume centre in worldspace
	worldCentre = Vec2(mySprite->getContentSize().width, mySprite->getContentSize().height) / 2;
	MyHelpers::transformPoint2(volumeTransform, &worldCentre);
	//get the volume extent
	volumeExtent = Vec2(mySprite->getContentSize().width / 2, 0);
	MyHelpers::transformVec2(volumeTransform, &volumeExtent);
	//get the volume extent Up, and apply it to find the Spawn direction edge
	volumeExtentUp = Vec2(0, mySprite->getContentSize().height / 2);
	spawnEdge = Vec2(0, spawnDirection.y*volumeExtentUp.y);
	MyHelpers::transformVec2(volumeTransform, &volumeExtentUp);
	MyHelpers::transformVec2(volumeTransform, &spawnEdge);

	//check for colisions between spawn component centre and extremeties
	PhysicsWorld *physWorld = getPhysicsBody()->getWorld();
	//cast 2 rays along the spawn edge, 1 along each direction, to get the minimum spawn area
	Vec2 posEdge = worldCentre + volumeExtent + spawnEdge,
		negEdge = worldCentre - volumeExtent + spawnEdge;
	//FindSpawnAreaEdge will store the shortest distance collision into the &edge Vector
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume::FindSpawnAreaEdge, this), worldCentre + spawnEdge, posEdge, static_cast<void*>(&posEdge));
	physWorld->rayCast(CC_CALLBACK_3(SpawnVolume::FindSpawnAreaEdge, this), worldCentre + spawnEdge, negEdge, static_cast<void*>(&negEdge));

	//recentre the vectors
	posEdge -= spawnEdge;
	negEdge -= spawnEdge;

	//desired new extent and component centre
	posEdge -= spawnUnitExtent;
	negEdge += spawnUnitExtent;
	spawnAreaExtentForward = (negEdge - posEdge) / 2;
	spawnAreaExtentUp = volumeExtentUp + spawnUnitExtentUp;
	spawnAreaCentre = negEdge - spawnAreaExtentForward;
	MyHelpers::transformPoint2(getWorldToNodeTransform(), &spawnAreaCentre);


	//////////////////////////////////////////////////
	////// TEST DRAW CODE
	//LevelManager::getInstance()->getCurrentLevel()->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	//volumeExtent = 2 * spawnAreaExtentForward;
	//volumeExtentUp = 2 * spawnAreaExtentUp;
	///*rotatedExtent.x = abs(rotatedExtent.x);
	//rotatedExtent.y = abs(rotatedExtent.y);*/
	//float myRotation = CC_DEGREES_TO_RADIANS(mySprite->getRotation());
	//volumeExtent = volumeExtent.rotateByAngle(Vec2::ZERO, myRotation);
	//volumeExtentUp = volumeExtentUp.rotateByAngle(Vec2::ZERO, myRotation);
	//Size testSpriteSize = Size(volumeExtent.x, volumeExtentUp.y);
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
	spawnPoint += spawnAreaCentre;
	MyHelpers::transformPoint2(getNodeToWorldTransform(), &spawnPoint);
	//add spawner's rotation to our random angle
	spawnAngle -= getRotation() + mySprite->getRotation();
	thrustVector = spawnDirection.rotateByAngle(Vec2::ZERO, CC_DEGREES_TO_RADIANS(spawnAngle));
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