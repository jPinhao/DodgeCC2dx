#ifndef __DODGEPLAYERCONTROLLER_H__
#define __DODGEPLAYERCONTROLLER_H__

#include "PlayerSingleController.h"

class DodgePlayerController : public PlayerSingleController
{
public:
	static DodgePlayerController* create(Player *owner);

	//setup input listening from the player
	void setupPlayerInput() override;
	void disablePlayerInput() override;
};
#endif