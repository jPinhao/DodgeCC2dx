#ifndef __DODGELEVEL_H__
#define __DODGELEVEL_H__

#include "cocos2d.h"
#include "Pellet.h"

class DodgeLevel : public cocos2d::Scene
{
	friend class LevelManager;

public:
	static DodgeLevel* createFromFile(const std::string& filename);
	Pellet* spawnPlayer();
	Pellet* findPlayerPawn() const;
	
CC_CONSTRUCTOR_ACCESS:
	Scene* loadSceneFromFile(const std::string& filename);
	bool initWithFile(const std::string& filename);
	// read the nodes added from the level file and initialize their game properties
	void processLevelActors(Node &loadedLevel, cocos2d::Vec2 ratioLoadedToScreen);

private:
	typedef cocos2d::Scene super;

	void onEnter() override;
	void onExit() override;

	cocos2d::Vec2 defaultSpawnPoint;
};

#endif

