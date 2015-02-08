#include "LevelManager.h"

LevelManager* LevelManager::getInstance()
{
	static LevelManager *managerInstance;
	if (managerInstance == nullptr)
	{
		managerInstance = new LevelManager();
	}
	return managerInstance;
}

//preload and 
bool LevelManager::initializeLevels()
{
	mainMenu = DodgeLevel::createFromFile("level_square.csb");//DodgeLevel::createFromFile("main_menu.csb");
	mainMenu->retain();

	allLevels.clear();
	allLevels.push_back(DodgeLevel::createFromFile("level_square.csb"));
	allLevels[0]->retain();
	allLevels.push_back(DodgeLevel::createFromFile("level_hexagon.csb"));
	allLevels[1]->retain();

	return true;
}

bool LevelManager::loadMainMenu()
{
	if (mainMenu != nullptr)
	{
		switchLevel(mainMenu);
		return true;
	}
	else return false;
}

bool LevelManager::loadLevel(UINT index)
{
	if (isLevelAvailable(index))
	{
		switchLevel(allLevels[index]);
		return true;
	}
	else return false;
}

bool LevelManager::isLevelAvailable(UINT index)
{
	if (index >= 0 && index < allLevels.size())
	{
		return true;
	}
	
	return false;
}

DodgeLevel* LevelManager::getCurrentLevel()
{
	DodgeLevel *currentLevel = nullptr;
	cocos2d::Director *director = cocos2d::Director::getInstance();
	if (director)
	{
		cocos2d::Scene *currentScene = director->getRunningScene();
		if (currentScene)
		{
			currentLevel = dynamic_cast<DodgeLevel*>(currentScene);
		}
	}
	return currentLevel;
}

void LevelManager::switchLevel(DodgeLevel *newLevel)
{
	cocos2d::Director *director = cocos2d::Director::getInstance();
	DodgeLevel *currentLevel = getCurrentLevel();
	if (currentLevel)
	{
		director->replaceScene(newLevel);
	}
	else director->runWithScene(newLevel);
}



LevelManager::LevelManager()
{
}

//release all level when deleting
LevelManager::~LevelManager()
{
	mainMenu->release();
	for (auto level : allLevels)
	{
		level->release();
	}
}