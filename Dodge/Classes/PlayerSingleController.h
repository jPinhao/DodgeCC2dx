#ifndef __PLAYERCONTROLLER_H__
#define __PLAYERCONTROLLER_H__

#include "SingleController.h"
#include "IPlayerController.h"

class Player;

class PlayerSingleController : public SingleController, public IPlayerController
{
public:
	static PlayerSingleController* create(Player *owner);
	//setup input listening from the player
	virtual void setupPlayerInput(){};
	virtual void disablePlayerInput(){};

CC_CONSTRUCTOR_ACCESS:
	bool init(Player *owner) override;
	PlayerSingleController();
	//start the updates
	virtual void startController() override;
	//stop the updates
	virtual void stopController() override;
	//clear pawn and player links
	virtual void clearAllDependants() override;

	Player *controllingPlayer;

private:
	typedef SingleController super;
};

#endif