#include "DodgePlayerController.h"
#include "Pellet.h"

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