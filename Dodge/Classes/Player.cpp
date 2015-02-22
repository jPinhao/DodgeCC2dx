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

	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
	if (eventDispatcher)
	{
		auto listener = cocos2d::EventListenerCustom::create(EVENT_PELLET_SPAWN, CC_CALLBACK_1(Player::changeScoreEvent, this));
		eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
		listener = cocos2d::EventListenerCustom::create(EVENT_PELLET_DIE, CC_CALLBACK_1(Player::changeScoreEvent, this));
		eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
	}

	return true;
}

void Player::setPlayerController(Controller *controller)
{
	if (controller != myController)
	{
		if (controller)
		{
			IPlayerController *playerCast = dynamic_cast<IPlayerController*>(controller);
			if (playerCast)
			{
				if (myController) myController->release();
				myController = controller;
				myController->retain();
			}
		}
		else
		{
			myController->release();
			myController = nullptr;
		}
		//else throw std::bad_cast("controller must implement IPlayerController interface");
	}
}

void Player::changeScoreEvent(cocos2d::EventCustom* scoreEvent)
{
	std::string eventName = scoreEvent->getEventName();
	cocos2d::Node* target = scoreEvent->getCurrentTarget();
	if (eventName.compare(EVENT_PELLET_DIE)==0)
	{
		PawnDeathEvent *castEvent = static_cast<PawnDeathEvent*>(scoreEvent);
		Player *pawnOwner = castEvent->getPawn()->getControllingPlayer();
		if (pawnOwner == this)
		{

		}
		//AI-pawn
		else if (pawnOwner == nullptr)
		{
			levelScore.totalKilled++;
			//score changed!
			cocos2d::Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_PLAYER_SCORE_CHANGE, &levelScore);
		}
	}
	else if (eventName.compare(EVENT_PELLET_SPAWN)==0)
	{
		Pawn* eventSender = static_cast<Pawn*>(scoreEvent->getUserData());
		if (!eventSender->getControllingPlayer())
		{
			levelScore.totalSpawned++;
			levelScore.maxAtOnce = MAX(levelScore.maxAtOnce, levelScore.totalSpawned - levelScore.totalKilled);
			//score changed!
			cocos2d::Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_PLAYER_SCORE_CHANGE, &levelScore);
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

Player::~Player()
{
	if (myController)
	{
		//TODO: something about the controller being left without a player here
		myController->release();
		myController = nullptr;
	}
}