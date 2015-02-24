#include "Pellet.h"
#include "DodgePlayerController.h"
#include "SpawnComponent.h"
#include "DamageComponent.h"
#include "StaticHelpers.h"
#include "CustomEvents.h"
#include "object_tags.h"

USING_NS_CC;

Pellet* Pellet::create(UseController defaultController/* = UseController::AI*/)
{
	Pellet *pellet = new(std::nothrow) Pellet();

	Controller *myController = nullptr;
	if (defaultController == UseController::AI) myController = DefaultAIController::create();
	else if (defaultController == UseController::PLAYER) myController = DefaultPlayerController::create(nullptr);

	if (pellet && pellet->initWithController(myController, Sprite::create("Dot.png")))
	{
		pellet->autorelease();
	}
	else CC_SAFE_DELETE(pellet);

	return pellet;
}

Pellet* Pellet::createWithController(Controller *pawnController)
{
	Pellet *pellet = new(std::nothrow) Pellet();
	if (pellet && pellet->initWithController(pawnController, Sprite::create("Dot.png")))
	{
		pellet->autorelease();
	}
	else CC_SAFE_DELETE(pellet);

	return pellet;
}

void Pellet::onEnter()
{
	super::onEnter();
	getEventDispatcher()->dispatchCustomEvent(EVENT_PELLET_SPAWN, this);
}

void Pellet::kill()
{
	PawnDeathEvent deathEvent(EVENT_PELLET_DIE, *this);
	//dispatch event before we die, otherwise no eventDispatcher available
	getEventDispatcher()->dispatchEvent(&deathEvent);
	super::kill();
}

void Pellet::postInitializeCustom(void* userData)
{
	Vec2 *moveDir = static_cast<Vec2*>(userData);
	if (moveDir) SetMovementDirection(*moveDir);
}

bool Pellet::initWithController(Controller* pawnController, Sprite *sprite)
{
	//Pellet's must have a sprite, Pawn's don't - check here
	if (!sprite) return false;

	if (super::initWithController(pawnController, sprite))
	{
		//initialize touPOsition for follow touch code
		touchPosition.set(PELLET_NOTARGETX, PELLET_NOTARGETY);
		//prepare spawn component
		SpawnComponent *spawnComponent = SpawnComponent::create();
		spawnComponent->setSpawnAnim("pellet_spawn.png", .6f, mySprite);
		addComponent(spawnComponent);

		//add ability to take damage
		TakesDamageComponent *damageCom = TakesDamageComponent::create(PELLET_LIFE);
		damageCom->onLifeUpdate = 
					[this,damageCom](float damageDone)
					{
						this->setScale((.7f * damageCom->getCurrentLife() / damageCom->getMaxLife()) + .3f);
					};
		addComponent(damageCom);

		return true;
	}
	else return false;
}

void Pellet::setupPhysicsBody()
{
	super::setupPhysicsBody();
	auto myBody = getPhysicsBody();
	if (myBody)
	{
		myBody->setVelocityLimit(moveSpeed);
		myBody->setDynamic(true);
		////start as disabled until we've fully spawned
		//myBody->setEnable(false);

		//setup the contact listener
		auto contactListener = EventListenerPhysicsContact::create();
		contactListener->onContactBegin = CC_CALLBACK_1(Pellet::onContactBegin, this);
		contactListener->onContactPostSolve = CC_CALLBACK_2(Pellet::onContactPostSolve, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
	}
}

void Pellet::updatePhysicsBodyShape()
{
	if (getPhysicsBody())
	{
		getPhysicsBody()->removeAllShapes();
		PhysicsShapeCircle *circle = PhysicsShapeCircle::create((getScale()*mySprite->getContentSize().width) / 2,
			PhysicsMaterial(0.f, 1.f, 0.f));
		circle->setContactTestBitmask(0xFFFFFFFF);
		getPhysicsBody()->addShape(circle);
	}
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
	PhysicsShape *myShape, *otherShape;
	// this get's called on any contact for every PhysicsListener, so we need to check if we are indeed one of the colliding entities....
	if (actor1 == this)
	{
		otherActor = actor2;
		myShape = contact.getShapeA();
		otherShape = contact.getShapeB();
	}
	else if (actor2 == this)
	{
		otherActor = actor1;
		myShape = contact.getShapeB();
		otherShape = contact.getShapeA();
	}
	else return true;

	//otherActor could be null - if other pellet get's flagged for destruction, it's body get's immedeately detached from the body
	if (!otherActor) return false;
	
	if (otherActor->isRunning() && this->isRunning())
	{
		//did we collide with another pellet?
		Pellet* otherPellet = dynamic_cast<Pellet*>(otherActor);
		if (otherPellet)
		{
			//player hit something, do nothing
			if (this->getControllingPlayer())
			{
				if (myShape->getTag() == TAG_GAME_COMNODE_AURADMG_SHAPE) return true;
				else return false;
			}
			//we just collided with the player, and it'stheir primary collision shape, kill them by applying max damage
			if (otherPellet->getControllingPlayer() && otherShape->getTag() != TAG_GAME_COMNODE_AURADMG_SHAPE)
			{
				TakesDamageComponent *otherDamageCom = dynamic_cast<TakesDamageComponent*>(otherPellet->getComponent("TakesDamageComponent"));
				if (otherDamageCom)
				{
					otherDamageCom->addDamage(otherDamageCom->getCurrentLife(), 0x0000);
				}
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

//onTouch triggers
void Pellet::setTargetPosition(const std::vector<Touch*>& touches, Event* event)
{
	CCLOG("Pellet::setTargetPosition");
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

Pellet::Pellet()
	: super(),
	touchPosition(PELLET_NOTARGETX, PELLET_NOTARGETY)
{

}

void Pellet::SetMovementDirection(cocos2d::Vec2 newDirection)
{
	moveDirection = newDirection;
}

const Pellet& Pellet::getDefaultObject()
{
	static Pellet* defaultPellet;
	if (defaultPellet == nullptr)
	{
		defaultPellet = Pellet::create();
		defaultPellet->retain();
	}
	return *defaultPellet;
}

Pellet* Pellet::clone() const
{
	Pellet* clonePellet = new (std::nothrow) Pellet(*this);
	if (clonePellet && clonePellet->initWithController(nullptr, clonePellet->mySprite))
	{
		clonePellet->autorelease();
	}
	else CC_SAFE_DELETE(clonePellet);
	return clonePellet;
}

Pellet::Pellet(const Pellet& copy)
	: super(copy),
	moveSpeed(copy.moveSpeed),
	touchPosition(PELLET_NOTARGETX, PELLET_NOTARGETY)
{

}