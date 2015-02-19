#ifndef __IPLAYERCONTROLLER_H__
#define __IPLAYERCONTROLLER_H__

class Player;

//Interface for player controllers
class IPlayerController
{
public:
	//PControllers should be able to enable/disable the player input
	virtual void setupPlayerInput() = 0;
	virtual void disablePlayerInput() = 0;

protected:
	//PControllers require a player owner to be initialized
	virtual bool init(Player *owner) = 0;
	//PControllers hold a player reference
	Player *controllingPlayer;
};

#endif