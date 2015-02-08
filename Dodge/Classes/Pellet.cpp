#include "Pellet.h"
#include "SpawnComponent.h"

USING_NS_CC;

Pellet* Pellet::create()
{
	Pellet *pellet = new(std::nothrow) Pellet();
	if (pellet && pellet->initWithFile("Dot.png"))
	{
		pellet->autorelease();
		return pellet;
	}
	else
	{
		delete pellet;
		pellet = NULL;
		return NULL;
	}
}

bool Pellet::initWithFile(const std::string& filename)
{
	//try to initialize the sprite
	if (!super::initWithFile(filename))
	{
		return false;
	}
	
	//anchor point mid
	setAnchorPoint(Vec2(.5f, .5f));
	//initialize touPOsition for follow touch code
	touchPosition.set(PELLET_NOTARGETX, PELLET_NOTARGETY);
	//prepare spawn component
	SpawnComponent *spawnComponent = SpawnComponent::create();
	spawnComponent->setSpawnAnim("pellet_spawn.png", .6f, getTexture());
	addComponent(spawnComponent);

	//pellet physics setup
	PhysicsBody *pelletBody = PhysicsBody::createCircle((getScale()*getContentSize().width) / 2,
															PhysicsMaterial(0.f, 1.f, 0.f));
	if (pelletBody)
	{
		pelletBody->setVelocityLimit(moveSpeed);
		pelletBody->setDynamic(true);
		setPhysicsBody(pelletBody);
		//start as disabled until we've fully spawned
		pelletBody->setEnable(false);

		return true;
	}

	return false;
}

void Pellet::update(float deltaTime)
{
	if (getPhysicsBody() && getPhysicsBody()->isEnabled())
	{
		if (touchPosition.x != PELLET_NOTARGETX && touchPosition.y != PELLET_NOTARGETY)
		{
			Vec2 velocity = touchPosition - getPosition();
			float myRadius = getContentSize().width*getScale()/2;
			if (velocity.length() <= myRadius) getPhysicsBody()->setVelocity(Vec2::ZERO);
			else getPhysicsBody()->setVelocity(velocity.getNormalized()*moveSpeed);
		}
	}
}

//onTouch triggers
void Pellet::setTargetPosition(const std::vector<Touch*>& touches, Event* event)
{
	if (getPhysicsBody() && getPhysicsBody()->isEnabled())
	{
		Vec2 newTargetPosition = Vec2(PELLET_NOTARGETX, PELLET_NOTARGETY);
		for (auto touch : touches)
		{
			if (touch)
			{
				newTargetPosition = Director::getInstance()->convertToGL(touch->getLocationInView());
			}
		}

		if (newTargetPosition.x != PELLET_NOTARGETX && newTargetPosition.y != PELLET_NOTARGETY)
		{
			touchPosition.set(newTargetPosition);
		}
	}
}

void Pellet::clearTargetPosition(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	touchPosition.set(PELLET_NOTARGETX, PELLET_NOTARGETY);
	if (getPhysicsBody() && getPhysicsBody()->isEnabled())
	{
		getPhysicsBody()->setVelocity(Vec2::ZERO);
	}
}