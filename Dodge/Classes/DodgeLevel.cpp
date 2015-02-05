#include "DodgeLevel.h"
#include "object_tags.h"
#include "Pellet.h"
#include "cocostudio\ActionTimeline\CSLoader.h"

USING_NS_CC;

DodgeLevel* DodgeLevel::createFromFile(const std::string& filename)
{
	DodgeLevel *level = new(std::nothrow) DodgeLevel();
	if (level && level->initWithFile(filename))
	{
		level->autorelease();
		return level;
	}
	else
	{
		delete level;
		level = NULL;
		return NULL;
	}
}

//init level from file
bool DodgeLevel::initWithFile(const std::string& filename)
{
	//if we can't create a basic scene with physics we failed
	if (!initWithPhysics())
	{
		return false;
	}
	else
	{
		//disable gravity
		getPhysicsWorld()->setGravity(Vec2(0.f, 0.f));
	}

	Node *levelRoot = CSLoader::createNode(filename);
	
	if (levelRoot)
	{
		// position our generated level in the centre and scale it to fill the scene
		levelRoot->setAnchorPoint(Vec2(.5f, .5f));
		levelRoot->setPosition(Vec2(getContentSize().width / 2, getContentSize().height / 2));
		//levelRoot->setScale(getContentSize().width / levelRoot->getContentSize().width, getContentSize().height / levelRoot->getContentSize().height);

		//process the Actors in the pre-made level
		processLevelActors(*levelRoot);

		/*Pellet *playerPellet = Pellet::create();
		playerPellet->setPosition(defaultSpawnPoint);
		levelRoot->addChild(playerPellet);*/

		addChild(levelRoot);

		return true;
	}
	// return the scene
	return false;
}

void DodgeLevel::processLevelActors(Node &loadedLevel)
{
	Vector<Node*> children = loadedLevel.getChildren();
	for (auto actor : children)
	{
		switch (actor->getTag())
		{
			case TAG_EDITOR_WALL:
			{
				Size actorContentSize = actor->getContentSize();
				PhysicsBody *wallBody = PhysicsBody::createBox(
					Size(actor->getScaleX()*actorContentSize.width, actor->getScaleY()*actorContentSize.height) / 2,
					PhysicsMaterial(0.f, 1.f, 0.f));
				if (wallBody)
				{
					wallBody->setEnable(false);
					wallBody->setDynamic(false);
					actor->setPhysicsBody(wallBody);
				}
				break;
			}
			case TAG_EDITOR_BG:
			{
				break;
			}
			case TAG_EDITOR_SPAWN :
				defaultSpawnPoint = actor->getPosition();
				break;
		}
	}
}