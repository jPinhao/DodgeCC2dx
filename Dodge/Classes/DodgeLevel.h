#ifndef __DODGELEVEL_H__
#define __DODGELEVEL_H__

#include "cocos2d.h"
#include "Pellet.h"
#include "SpawnController.h"
#include "object_tags.h"

class LevelManager;
class SpawnController;
class SpawnVolume;

class DodgeLevel : public cocos2d::Scene
{
	friend class LevelManager;

public:
	static DodgeLevel* createFromFile(const std::string& filename);
	//transform a coordinate from the level editor into one in our current game window
	static cocos2d::Vec2 editorToGameCoordinateTransform(const cocos2d::Vec2& positionInEditor, const cocos2d::Vec2& levelSizeInEditor);
	Pellet* spawnPlayer();
	Pellet* findPlayerPawn() const;

	Pellet* spawnUnit(Pellet* newUnit, cocos2d::Vec2 position);
	
	SpawnController* findControllerForSpawner(SpawnVolume* spawner) const;

CC_CONSTRUCTOR_ACCESS:
	Scene* loadSceneFromFile(const std::string& filename);
	bool initWithFile(const std::string& filename);
	// read the nodes added from the level file and initialize their game properties
	void processLevelActors(Node *loadedLevel);

private:
	typedef cocos2d::Scene super;

	void onEnter() override;
	void onExit() override;

	cocos2d::Vec2 defaultSpawnPoint;
};

#endif

