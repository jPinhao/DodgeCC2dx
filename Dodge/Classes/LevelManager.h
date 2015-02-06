#ifndef __LEVELMANAGER_H__
#define __LEVELMANAGER_H__

#include "cocos2d.h"
#include "DodgeLevel.h"

class LevelManager //: public cocos2d::Ref
{
public:
	static LevelManager* getInstance();
	//preload all levels ready for play
	bool initializeLevels();
	//switch to the main menu
	bool loadMainMenu();
	//switch to a particular level
	bool loadLevel(UINT index);
	//check if a level is available
	bool isLevelAvailable(UINT index);

CC_CONSTRUCTOR_ACCESS:
	LevelManager();
	LevelManager(LevelManager const &) = delete;
	void operator=(LevelManager const &) = delete;

	~LevelManager();

private:
	//actually proceed with switching out the current level and adding a new one
	void switchLevel(DodgeLevel *newLevel);

	std::vector<DodgeLevel*> allLevels;
	DodgeLevel *mainMenu;
};

#endif