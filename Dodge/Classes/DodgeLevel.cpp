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
		Layer *foreground = Layer::create();
		foreground->setTag(TAG_GAME_LAYER_FOREGROUND);
		addChild(foreground, 0);
		Layer *ui = Layer::create();
		ui->setTag(TAG_GAME_LAYER_UI);
		addChild(ui, 1);

		//process the Actors in the pre-made level
		processLevelActors(*levelRoot, Vec2(getContentSize().width / levelRoot->getContentSize().width, getContentSize().height / levelRoot->getContentSize().height));
		levelRoot = nullptr;

		////instantiate the player pellet
		//Pellet *playerPellet = Pellet::create();
		//playerPellet->setPosition(defaultSpawnPoint);
		//foreground->addChild(playerPellet, 0);

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
	Node *foreground = getChildByTag(TAG_GAME_LAYER_FOREGROUND);
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

void DodgeLevel::OnLevelStart(){}
void DodgeLevel::OnLevelRemove(){}