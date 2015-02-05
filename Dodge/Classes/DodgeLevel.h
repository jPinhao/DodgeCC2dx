#ifndef __DODGELEVEL_H__
#define __DODGELEVEL_H__

#include "cocos2d.h"

class DodgeLevel : public cocos2d::Scene
{
public:
	static DodgeLevel* createFromFile(const std::string& filename);

CC_CONSTRUCTOR_ACCESS:
	bool initWithFile(const std::string& filename);
	void processLevelActors(Node &loadedLevel);

private:
	typedef cocos2d::Scene super;

	cocos2d::Vec2 defaultSpawnPoint;

};

#endif

