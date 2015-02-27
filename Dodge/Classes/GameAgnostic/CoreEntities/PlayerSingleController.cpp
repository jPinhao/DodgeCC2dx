#include "PlayerSingleController.h"
#include "Player.h"

USING_NS_CC;

PlayerSingleController* PlayerSingleController::create(Player *owner)
{
	PlayerSingleController *ret = new(std::nothrow) PlayerSingleController();
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

bool PlayerSingleController::init(Player *owner)
{
	if (owner)
	{
		if (super::init())
		{
			owner->setPlayerController(this);
			controllingPlayer = owner; 
			return true;
		}
	}
	return false;
}

//start the updates
void PlayerSingleController::startController()
{
	super::startController();
	playerPawnSetup();
}

//stop the updates
void PlayerSingleController::stopController()
{
	super::stopController();
	playerPawnRelease();
}

void PlayerSingleController::clearAllDependants()
{
	super::clearAllDependants();
	controllingPlayer->setPlayerController(nullptr);
	controllingPlayer = nullptr;
}


PlayerSingleController::PlayerSingleController()
	:super(),
	controllingPlayer(nullptr)
{

}