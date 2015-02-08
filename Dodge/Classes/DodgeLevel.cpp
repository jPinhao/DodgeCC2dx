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

	auto director = Director::getInstance();
	Size screenSize = director->getWinSize();
	Node *levelRoot = CSLoader::createNode(filename);
	if (levelRoot)
	{
		//add split game render layers
		Layer *background = Layer::create();
		background->setTag(TAG_GAME_LAYER_BACKGROUND);
		addChild(background, -2);
		Layer *scenery = Layer::create();
		scenery->setTag(TAG_GAME_LAYER_SCENERY);
		addChild(scenery, -1);
		Layer *playerLayer = Layer::create();
		playerLayer->setTag(TAG_GAME_LAYER_PLAYER);
		addChild(playerLayer, 0);
		Layer *ui = Layer::create();
		ui->setTag(TAG_GAME_LAYER_UI);
		addChild(ui, 1);

		//process the Actors in the pre-made level
		processLevelActors(*levelRoot, Vec2(getContentSize().width / levelRoot->getContentSize().width, getContentSize().height / levelRoot->getContentSize().height));
		levelRoot = nullptr;

		return true;
	}
	// return the scene
	return false;
}

// read the nodes added from the level file and initialize their game properties
// @processRoot - root of the loaded level
// @ratioLoadedToScreen - ratio of original level size and current game screen size
// @TODO - actually get the screen resizing and actor repositioning to work properly
void DodgeLevel::processLevelActors(Node &processRoot, Vec2 ratioLoadedToScreen)
{
	Node *background = getChildByTag(TAG_GAME_LAYER_BACKGROUND);
	Node *scenery = getChildByTag(TAG_GAME_LAYER_SCENERY);
	Node *playerLayer = getChildByTag(TAG_GAME_LAYER_PLAYER);
	Node *ui = getChildByTag(TAG_GAME_LAYER_UI);

	Vector<Node*> children = processRoot.getChildren();
	processRoot.removeAllChildrenWithCleanup(false);
	for (auto actor : children)
	{
		actor->setPosition(actor->getPosition().x * ratioLoadedToScreen.x, actor->getPosition().y * ratioLoadedToScreen.y);

		switch (actor->getTag())
		{
			case TAG_EDITOR_WALL:
			{
				scenery->addChild(actor);

				Size actorContentSize = actor->getContentSize();
				PhysicsBody *wallBody = PhysicsBody::createBox(
					Size(actor->getScaleX()*actorContentSize.width, actor->getScaleY()*actorContentSize.height) / 2,
					PhysicsMaterial(0.f, 1.f, 0.f));
				if (wallBody)
				{
					wallBody->setEnable(false);
					wallBody->setDynamic(false);
					//actor->setPhysicsBody(wallBody);
				}
				break;
			}
			case TAG_EDITOR_BG:
			{
				background->addChild(actor);
				break;
			}
			case TAG_EDITOR_BG_SCORE:
			{
				background->addChild(actor);
				break;
			}
			case TAG_EDITOR_SPAWN:
			{
				defaultSpawnPoint = actor->getPosition();
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void DodgeLevel::onEnter()
{
	super::onEnter();
	Pellet *playerPawn = spawnPlayer();
	CCASSERT(playerPawn, "ERROR: tried to start a level but couldn't spawn the player");
}

void DodgeLevel::onExit()
{
	super::onExit();
	Node *player = findPlayerPawn();
	if (player) removeChild(player);
}

Pellet* DodgeLevel::spawnPlayer()
{
	Node *playerLayer = getChildByTag(TAG_GAME_LAYER_PLAYER);
	CCASSERT(playerLayer, "ERROR: Couldn't find the player layer to spawn");
	if (playerLayer)
	{
		Pellet *playerPawn = findPlayerPawn();
		CCASSERT(playerPawn == nullptr, "ERROR: Trying to spawn the player when a pawn is already in place");
		if (!playerPawn)
		{
			playerPawn = Pellet::create();
			playerPawn->setPosition(defaultSpawnPoint);
			playerLayer->addChild(playerPawn);
		}

		//setup the player input (should build some sort of player controller)
		auto listener = EventListenerTouchAllAtOnce::create();
		listener->onTouchesBegan = CC_CALLBACK_2(Pellet::setTargetPosition, playerPawn);
		listener->onTouchesMoved = CC_CALLBACK_2(Pellet::setTargetPosition, playerPawn);
		listener->onTouchesEnded = CC_CALLBACK_2(Pellet::clearTargetPosition, playerPawn);
		listener->onTouchesCancelled = CC_CALLBACK_2(Pellet::clearTargetPosition, playerPawn);

		auto dispatcher = playerPawn->getEventDispatcher();
		dispatcher->addEventListenerWithSceneGraphPriority(listener, playerPawn);

		return playerPawn;
	}
	
	return nullptr;
}

Pellet* DodgeLevel::findPlayerPawn() const
{
	Node *playerLayer = getChildByTag(TAG_GAME_LAYER_PLAYER);
	if (playerLayer)
	{
		Node *playerNode = playerLayer->getChildByTag(TAG_GAME_PLAYERPAWN);
		if (playerNode)
		{
			return dynamic_cast<Pellet*>(playerNode);
		}
	}
	return nullptr;
}