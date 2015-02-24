#include "DodgePlayerController.h"
#include "Pellet.h"
#include "DamageComponent.h"

USING_NS_CC;

DodgePlayerController* DodgePlayerController::create(Player *owner)
{
	DodgePlayerController *ret = new(std::nothrow) DodgePlayerController();
	if (ret && ret->init(owner))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

//do any player pawn specific setup
void DodgePlayerController::playerPawnSetup()
{
	super::playerPawnSetup();
	if (myPawn)
	{
		//add a damaging aura
		float auraRadius = (myPawn->getContentSize().width * myPawn->getScaleX()) * 3 / 2;
		auto doDamageComponent = AuraDamageComponent::create(auraRadius, PELLET_DECAY_RATE, PELLET_DECAY_RATE, DMG_AI_OTHER, 0x0000);
		myPawn->addComponent(doDamageComponent);
		//also disable restitution on the Player pellet, we shouldn't be bouncy like the others
		if (myPawn->getPhysicsBody())
		{
			myPawn->getPhysicsBody()->getFirstShape()->setMaterial(PhysicsMaterial(0.f, 0.f, 0.f));
		}
	}
}
//clear any player specific state
void DodgePlayerController::playerPawnRelease()
{
	super::playerPawnRelease();
	if (myPawn)
	{
		auto aura = myPawn->getComponent("AuraDamageComponent");
		if (aura) myPawn->removeComponent(aura);
		//also re-enable restitution on the pellet
		if (myPawn->getPhysicsBody())
		{
			myPawn->getPhysicsBody()->getFirstShape()->setMaterial(PhysicsMaterial(0.f, 1.f, 0.f));
		}
	}
}

//setup input listening from the player
void DodgePlayerController::setupPlayerInput()
{
	CCLOG("DodgePlayerController::setupPlayerInput()");
	if (myPawn)
	{
		Pellet* myPellet = dynamic_cast<Pellet*>(myPawn);
		if (myPellet)
		{
			CCLOG("DodgePlayerController::setupPlayerInput() !!2!!");
			auto listener = EventListenerTouchAllAtOnce::create();
			listener->onTouchesBegan = CC_CALLBACK_2(Pellet::setTargetPosition, myPellet);
			listener->onTouchesMoved = CC_CALLBACK_2(Pellet::setTargetPosition, myPellet);
			listener->onTouchesEnded = CC_CALLBACK_2(Pellet::clearTargetPosition, myPellet);
			listener->onTouchesCancelled = CC_CALLBACK_2(Pellet::clearTargetPosition, myPellet);

			auto dispatcher = getEventDispatcher();
			dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
		}
	}
}

void DodgePlayerController::disablePlayerInput()
{
	CCLOG("DodgePlayerController::disablePlayerInput()");
	getEventDispatcher()->removeEventListenersForType(EventListener::Type::TOUCH_ALL_AT_ONCE);
	return;
}