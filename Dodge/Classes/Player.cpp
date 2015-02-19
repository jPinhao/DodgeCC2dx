#include "Player.h"
#include "PlayerSingleController.h"
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
		else throw std::bad_cast("controller must implement IPlayerController interface");
	}
	else myController = nullptr;
}

Controller* Player::getController()
{
	return myController;
}

Player::Player()
	: myController(nullptr)
{

}