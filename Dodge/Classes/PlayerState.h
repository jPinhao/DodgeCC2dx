#ifndef __PLAYERSTATE__H_
#define __PLAYERSTATE__H_

#include <string>
#include "CCStdC.h"

class PlayerState
{
public:
	static PlayerState* create(unsigned int ID, const std::string& name = "");
	virtual bool saveState()=0;

CC_CONSTRUCTOR_ACCESS:
	bool init(unsigned int ID, const std::string& name = "");
	PlayerState(const PlayerState&) = delete;
	PlayerState &operator=(const PlayerState&) = delete;

private:
	//player identifiers
	unsigned int playerID;
	std::string playerName;

	//in-game state
	float playerScore;
};

#endif