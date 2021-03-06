#ifndef __DODGEPLAYERCONTROLLER_H__
#define __DODGEPLAYERCONTROLLER_H__

#include "CoreEntities\PlayerSingleController.h"

class DodgePlayerController : public PlayerSingleController
{
public:
	static DodgePlayerController* create(Player *owner);

	//setup input listening from the player
	void setupPlayerInput() override;
	void disablePlayerInput() override;

protected:
	//do any player pawn specific setup
	void playerPawnSetup() override;
	//clear any player specific state
	void playerPawnRelease() override;

private:
	typedef PlayerSingleController super;
};
#endif