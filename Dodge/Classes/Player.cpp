#include "Player.h"
#include "PlayerSingleController.h"
#include "CustomEvents.h"
#include "Pellet.h"
#include <exception>

Player* Player::create(unsigned int ID, const std::string& name/* = ""*/)
{
	auto ret = new (std::nothrow)Player();
	if (ret && ret->init(ID, name))
	{
	}
	else CC_SAFE_DELETE(ret);
	return ret;
}

bool Player::init(unsigned int ID, const std::string& name)
{
	playerID = ID;
	playerName = name;

	auto pelletSpawnListener = cocos2d::EventListenerCustom::create(EVENT_PELLET_SPAWN, CC_CALLBACK_1(Player::changeScoreEvent,this));
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(pelletSpawnListener, 1);

	return true;
}

void Player::setPlayerController(Controller *controller)
{
	if (controller)
	{
		IPlayerController *playerCast = dynamic_cast<IPlayerController*>(controller);
		if (playerCast)
		{
			myController = controller;
		}
		//else throw std::bad_cast("controller must implement IPlayerController interface");
	}
	else myController = nullptr;
}

void Player::changeScoreEvent(cocos2d::EventCustom* scoreEvent)
{
	std::string eventName = scoreEvent->getEventName();
	cocos2d::Node* target = scoreEvent->getCurrentTarget();
	if (scoreEvent->getUserData())
	{
		if (eventName.compare(EVENT_PELLET_DIE)==0)
		{
			Pellet* eventSender = static_cast<Pellet*>(scoreEvent->getUserData());
			if (!eventSender->isPlayerPawn()) 
			{
				levelScore.totalKilled++;
				//score changed!
				cocos2d::Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_PLAYER_SCORE_CHANGE, &levelScore);
			}
		}
		else if (eventName.compare(EVENT_PELLET_SPAWN)==0)
		{
			Pellet* eventSender = static_cast<Pellet*>(scoreEvent->getUserData());
			if (!eventSender->isPlayerPawn())
			{
				levelScore.totalSpawned++;
				levelScore.maxAtOnce = MAX(levelScore.maxAtOnce, levelScore.totalSpawned - levelScore.totalKilled);
				//score changed!
				cocos2d::Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_PLAYER_SCORE_CHANGE, &levelScore);
			}
		}
	}
	return;
}

Controller* Player::getController()
{
	return myController;
}

Player::Player()
	: myController(nullptr),
	levelScore(),
	highscores()
{

}