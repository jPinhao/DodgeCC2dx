#ifndef __PLAYERMANAGER__H_
#define __PLAYERMANAGER__H_

#include "Player.h"
#include <vector>
#include <string>

// Keeps track of the players inside the game
class PlayerManager
{
public:
	//return the game's player manager instance
	static PlayerManager* getInstance();
	//add a new player - id must be provided (possibly self-generated here?)
	bool addPlayer(unsigned int id, std::string name = "");
	//get a player by ID
	Player* getPlayer(unsigned int id);

private:
	std::vector<Player*> allPlayers;
};

#endif