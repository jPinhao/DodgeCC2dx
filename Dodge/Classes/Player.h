#ifndef __PLAYER__H_
#define __PLAYER__H_

#include "cocos2d.h"

struct SessionScore
{
	unsigned int maxKilled;
	unsigned int maxSpawned;
	unsigned int totalKilled;
	unsigned int totalSpawned;
	unsigned int maxAtOnce;
};
struct LevelScore
{
	unsigned int totalKilled;
	unsigned int totalSpawned;
	unsigned int maxAtOnce;
};

class Controller;

class Player
{
	friend class PlayerManager;

public:
	static Player* create(unsigned int ID, const std::string& name = "");
	//virtual bool saveState() = 0;
	void setPlayerController(Controller *controller);
	Controller* getController();

CC_CONSTRUCTOR_ACCESS:
	bool init(unsigned int ID, const std::string& name);
	Player();
	Player(const Player&) = delete;
	Player &operator=(const Player&) = delete;

private:
	//private destructor - PlayerManager has control over my life time
	~Player(){};

	//player identifiers
	unsigned int playerID;
	std::string playerName;

	//in-game state
	Controller *myController;
	SessionScore highscores;
	LevelScore levelScore;
};

#endif