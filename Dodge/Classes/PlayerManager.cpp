#include "PlayerManager.h"

PlayerManager* PlayerManager::getInstance()
{
	static PlayerManager* myInstance = nullptr;
	if (!myInstance)
	{
		myInstance = new PlayerManager();
	}
	return myInstance;
}

bool PlayerManager::addPlayer(unsigned int id, std::string name/*= ""*/)
{
	for (unsigned int index = 0; index < allPlayers.size(); index++)
	{
		if (allPlayers[index]->playerID == id) return true;
	}

	Player *newPlayer = Player::create(id, name);
	if (newPlayer)
	{
		allPlayers.push_back(newPlayer);
		return true;
	}
	else return false;
}

Player* PlayerManager::getPlayer(unsigned int id)
{
	for (unsigned int index = 0; index < allPlayers.size(); index++)
	{
		if (allPlayers[index]->playerID == id) return allPlayers[index];
	}
	return nullptr;
}