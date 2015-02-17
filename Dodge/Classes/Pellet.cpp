#include "Pellet.h"
#include "SingleController.h"
#include "PlayerController.h"
#include "SpawnComponent.h"
#include "StaticHelpers.h"

USING_NS_CC;

Pellet* Pellet::create(UseController defaultController/* = UseController::AI*/)
{
	Pellet *pellet = new(std::nothrow) Pellet();
	if (pellet && pellet->init(defaultController))
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

Pellet* Pellet::clone() const
{
	return Pellet::create();
}

void Pellet::postInitializeCustom(void* userData)
{
	Vec2 *moveDir = reinterpret_cast<Vec2*>(userData);
	if (moveDir) SetMovementDirection(*moveDir);
}

bool Pellet::init(UseController defaultController)
{
	Sprite *pelletSprite = Sprite::create("Dot.png");
	//try to initialize the sprite
	if (!pelletSprite)
	{
		return false;
	}
	
	//initialize touPOsition for follow touch code
	touchPosition.set(PELLET_NOTARGETX, PELLET_NOTARGETY);
	//prepare spawn component
	SpawnComponent *spawnComponent = SpawnComponent::create();
	spawnComponent->setSpawnAnim("pellet_spawn.png", .6f, pelletSprite);
	addComponent(spawnComponent);

	//pellet physics setup
	PhysicsBody *pelletBody = PhysicsBody::create();
	if (pelletBody)
	{
		pelletBody->setVelocityLimit(moveSpeed);
		pelletBody->setDynamic(true);
		setPhysicsBody(pelletBody);
		//start as disabled until we've fully spawned
		pelletBody->setEnable(false);

		//setup the contact listener
		auto contactListener = EventListenerPhysicsContact::create();
		contactListener->onContactBegin = CC_CALLBACK_1(Pellet::onContactBegin, this);
		contactListener->onContactPostSolve = CC_CALLBACK_2(Pellet::onContactPostSolve, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
	}

	Controller *myController = nullptr;
	if (defaultController == UseController::AI) myController=DefaultAIController::create();
	else if (defaultController == UseController::PLAYER) myController = DefaultPlayerController::create();

	return super::initWithController(myController, pelletSprite);
}

void Pellet::updatePhysicsBodyShape()
{
	getPhysicsBody()->removeAllShapes();
	PhysicsShapeCircle *circle = PhysicsShapeCircle::create((getScale()*mySprite->getContentSize().width) / 2,
																PhysicsMaterial(0.f, 1.f, 0.f));
	circle->setContactTestBitmask(0xFFFFFFFF);
	//circle->setContactTestBitmask(0xFFFFFFFF);
	getPhysicsBody()->addShape(circle);
}

void Pellet::update(float deltaTime)
{
	auto myPhysicsBody = getPhysicsBody();
	if (myPhysicsBody && myPhysicsBody->isEnabled())
	{
		myPhysicsBody->resetForces();
		//are we the player pawn and have input?
		if (touchPosition.x != PELLET_NOTARGETX && touchPosition.y != PELLET_NOTARGETY)
		{
			moveDirection = touchPosition - getPosition();
			float myRadius = getContentSize().width*getScale()/2;
			//ensure we are touching outside the pellet to avoid back/forw movement
			if (moveDirection.length() <= myRadius)
			{
				myPhysicsBody->setVelocity(Vec2::ZERO);
				moveDirection == Vec2::ZERO;
			}
		}
		//apply the moveDirection
		if (moveDirection != Vec2::ZERO)
		{
			Vec2 velocity = moveDirection.getNormalized()*moveSpeed - myPhysicsBody->getVelocity();
			Vec2 newForce = myPhysicsBody->getMass()*velocity / deltaTime;
			myPhysicsBody->applyForce(newForce);

			moveDirection = Vec2::ZERO;
		}
	}
}


// check if this is a player vs other pellet to kill the player and ignore collision
bool Pellet::onContactBegin(cocos2d::PhysicsContact &contact)
{
	Node *actor1 = MyHelpers::getPhysShapeOwner(contact.getShapeA());
	Node *actor2 = MyHelpers::getPhysShapeOwner(contact.getShapeB());
	Node *otherActor = nullptr;

	// this get's called on any contact for every PhysicsListener, so we need to check if we are indeed one of the colliding entities....
	if (actor1 == this) otherActor = actor2;
	else if (actor2 == this) otherActor = actor1;
	else return true;

	//otherActor could be null - if other pellet get's flagged for destruction, it's body get's immedeately detached from the body
	if (!otherActor) return false;
	
	if (otherActor->isRunning() && this->isRunning())
	{
		//did we collide with another pellet?
		Pellet* otherPellet = dynamic_cast<Pellet*>(otherActor);
		if (otherPellet)
		{
			if (this->isPlayerPawn())
			{
				//oh no, we be dead!
				this->removeFromParent();
				return false;
			}
			//we just collided with the player, ignore, other pellet will destroy itself and we don't collide
			else if (otherPellet->isPlayerPawn())
			{
				return false;
			}
		}
	}
	//one of the pellets is bound to be removed, do nothing
	else return false;
	
	//all good, we can proceed with the colision
	return true;
}

// ensure pellet exits collision with the correct speed
void Pellet::onContactPostSolve(cocos2d::PhysicsContact& contact, const cocos2d::PhysicsContactPostSolve& solve)
{
	Node *me = MyHelpers::getPhysShapeOwner(contact.getShapeA());
	if (this != me) me = MyHelpers::getPhysShapeOwner(contact.getShapeB());
	if (this != me) return;

	Vec2 newVelocity = getPhysicsBody()->getVelocity();
	if (newVelocity.length() != moveSpeed) SetMovementDirection(newVelocity);
}

bool Pellet::isPlayerPawn() const
{
	//temp hack to check if player
	float restitution = getPhysicsBody()->getFirstShape()->getRestitution();
	if (restitution == 0.f) return true;
	else return false;
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
	auto myPhysicsBody = getPhysicsBody();
	if (myPhysicsBody)
	{
		moveDirection = Vec2::ZERO;
		myPhysicsBody->resetForces();
		myPhysicsBody->setVelocity(Vec2::ZERO);
	}
}

void Pellet::SetMovementDirection(cocos2d::Vec2 newDirection)
{
	moveDirection = newDirection;
}