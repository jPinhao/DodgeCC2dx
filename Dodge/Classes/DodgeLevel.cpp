#include "DodgeLevel.h"
#include "PlayerManager.h"
#include "Player.h"
#include "DodgePlayerController.h"
#include "Pellet.h"
#include "SpawnVolume.h"
#include "CustomEvents.h"
#include "cocostudio\ActionTimeline\CSLoader.h"
#include "ui\UIText.h"

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
		processLevelActors(levelRoot);
		levelRoot = nullptr;

		return true;
	}
	// return the scene
	return false;
}

//transform a coordinate from the level editor into one in our current game window
Vec2 DodgeLevel::editorToGameCoordinateTransform(const Vec2& positionEditor,const Vec2& levelSizeEditor)
{
	//visible window origin in Design Resolution space
	Vec2 windowOrigin = Director::getInstance()->getVisibleOrigin();
	//visible window size
	Size windowSize = Director::getInstance()->getVisibleSize();
	
	Vec2 relativePos = Vec2(positionEditor.x / levelSizeEditor.x,
		positionEditor.y / levelSizeEditor.y);
	Vec2 newPosition = Vec2(relativePos.x * windowSize.width + windowOrigin.x,
		relativePos.y * windowSize.height + windowOrigin.y);

	return newPosition;
}

// read the nodes added from the level file and initialize their game properties
// @processRoot - root of the loaded level
// @ratioLoadedToScreen - ratio of original level size and current game screen size
// @TODO - actually get the screen resizing and actor repositioning to work properly
void DodgeLevel::processLevelActors(Node *processRoot)
{
	Node *background = getChildByTag(TAG_GAME_LAYER_BACKGROUND);
	Node *scenery = getChildByTag(TAG_GAME_LAYER_SCENERY);
	Node *playerLayer = getChildByTag(TAG_GAME_LAYER_PLAYER);
	Node *ui = getChildByTag(TAG_GAME_LAYER_UI);

	SpawnController *spawnController = SpawnController::create();
	spawnController->setTag(TAG_EDITOR_SPAWNCONTROLLER);
	background->addChild(spawnController);

	Vector<Node*> children = processRoot->getChildren();
	processRoot->removeAllChildrenWithCleanup(false);
	Vec2 levelSizeEditor = processRoot->getContentSize();
	Vec2 inGamePosition;
	for (auto actor : children)
	{
		inGamePosition = editorToGameCoordinateTransform(actor->getPosition(), levelSizeEditor);
		actor->setPosition(inGamePosition);
		switch (actor->getTag())
		{
			case TAG_EDITOR_SPAWNER:
			{
				float rotation = actor->getRotationSkewX();
				float scaleX = actor->getScaleX();
				//for some reason keeping the level loaded sprite messes everything's positions...
				actor = Sprite::create("dodge_wall.png");
				if (actor)
				{
					actor->setScaleX(scaleX);
					actor->setRotation(rotation);
					SpawnVolume *spawnVolume = SpawnVolume::createWithSprite(Pellet::getDefaultObject(), static_cast<Sprite*>(actor), spawnController);
					spawnVolume->setPosition(inGamePosition);
					scenery->addChild(spawnVolume);
				}

				break;
			}
			case TAG_EDITOR_BG:
			{
				//recreate a full-screen background with a tiling texture
				Vec2 windowOrigin = Director::getInstance()->getVisibleOrigin();
				Size windowSize = Director::getInstance()->getVisibleSize();
				Sprite* backgroundSprite = Sprite::create("dodge_background.png", 
					Rect(windowOrigin.x, windowOrigin.y,
					windowSize.width,
					windowSize.height));
				backgroundSprite->setAnchorPoint(Vec2(.5f, .5f));
				backgroundSprite->setPosition((windowOrigin+windowSize)/2);
				Texture2D::TexParams params = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
				backgroundSprite->getTexture()->setTexParameters(params);
				background->addChild(backgroundSprite);

				break;
			}
			case TAG_EDITOR_BG_SCORE:
			{
				ui::Text *bgText = dynamic_cast<ui::Text*>(actor);
				if (bgText)
				{
					background->addChild(bgText);
					auto scoreChangeListener = cocos2d::EventListenerCustom::create(EVENT_PLAYER_SCORE_CHANGE,
						[bgText](EventCustom* event)
						{
							LevelScore *score = static_cast<LevelScore*>(event->getUserData());
							Value scoreString((int)(score->totalSpawned + score->totalKilled));
							bgText->setString(scoreString.asString());
						});
					bgText->getEventDispatcher()->addEventListenerWithSceneGraphPriority(scoreChangeListener, bgText);
				}
				break;
			}
			case TAG_EDITOR_PLAYER_SPAWNPOINT:
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
	Pawn *playerPawn = spawnPlayer(0);
	CCASSERT(playerPawn, "ERROR: tried to start a level but couldn't spawn the player");
}

//remove player, do any transitions, save any states
void DodgeLevel::onExit()
{
	super::onExit();
	Player *player = PlayerManager::getInstance()->getPlayer(0);
	if (player)
	{
		Controller *controller = player->getController();
		if (controller)
		{
			Pawn *playerPawn = controller->getPawn();
			if (playerPawn) removeChild(playerPawn);
		}
	}	
}

Pawn* DodgeLevel::spawnPlayer(int playerID)
{
	Player *player = PlayerManager::getInstance()->getPlayer(playerID);
	Controller *controller = nullptr;
	Pawn *playerPawn = nullptr;
	if (player)
	{
		controller = player->getController();
		if (!controller) controller = DodgePlayerController::create(player);

		if (controller)
		{
			playerPawn = controller->getPawn();
			CCASSERT(playerPawn == nullptr, "ERROR: Trying to spawn the player when a pawn is already in place");
			if (!playerPawn)
			{
				playerPawn = spawnUnit(Pellet::createWithController(controller), defaultSpawnPoint);
			}

			CCASSERT(!(playerPawn == nullptr), "ERROR: Failed to spawn the player");
			if (playerPawn)
			{
				//also disable restitution on the Player pellet, we shouldn't be bouncy like the others
				if (playerPawn->getPhysicsBody())
				{
					playerPawn->getPhysicsBody()->getFirstShape()->setMaterial(PhysicsMaterial(0.f, 0.f, 0.f));
				}
			}
		}
	}
	return playerPawn;
}

Pawn* DodgeLevel::spawnUnit(Pawn* newUnit, cocos2d::Vec2 position)
{
	Node *playerLayer = getChildByTag(TAG_GAME_LAYER_PLAYER);
	if (newUnit)
	{
		newUnit->setPosition(position);
		playerLayer->addChild(newUnit);
	}
	return newUnit;
}

Pawn* DodgeLevel::findPlayerPawn() const
{
	Node *playerLayer = getChildByTag(TAG_GAME_LAYER_PLAYER);
	if (playerLayer)
	{
		Node *playerNode = playerLayer->getChildByTag(TAG_GAME_PLAYERPAWN);
		if (playerNode)
		{
			return dynamic_cast<Pawn*>(playerNode);
		}
	}
	return nullptr;
}

void DodgeLevel::addController(Controller* controller)
{
	Node *controllerLayer = getChildByTag(TAG_GAME_LAYER_BACKGROUND);
	if (controllerLayer)
	{
		controllerLayer->addChild(controller);
	}
}