#include "PlayerController.h"
#include "Pellet.h"

USING_NS_CC;

PlayerController* PlayerController::create()
{
	PlayerController *ret = new(std::nothrow) PlayerController();
	if (ret && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

//start the updates
void PlayerController::startController()
{
	super::startController();
	setupPlayerInput();
}

//stop the updates
void PlayerController::stopController()
{
	super::stopController();
	disablePlayerInput();
}

//setup input
void PlayerController::setupPlayerInput()
{
	if (myPawn)
	{
		Pellet* myPellet = dynamic_cast<Pellet*>(myPawn);
		if (myPellet)
		{
			//setup the player input and events (should build some sort of player controller)
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

void PlayerController::disablePlayerInput()
{
	getEventDispatcher()->removeEventListenersForType(EventListener::Type::TOUCH_ALL_AT_ONCE);
	return;
}


PlayerController::PlayerController()
	:super()
{

}