#ifndef __PLAYERCONTROLLER_H__
#define __PLAYERCONTROLLER_H__

#include "SingleController.h"

class PlayerController : public SingleController
{
public:
	static PlayerController* create();

CC_CONSTRUCTOR_ACCESS:
	PlayerController();
	//start the updates
	virtual void startController() override;
	//stop the updates
	virtual void stopController() override;

	//setup input listening from the player
	virtual void setupPlayerInput();
	virtual void disablePlayerInput();

private:
	typedef SingleController super;
};

#endif