#ifndef __SPAWNCOMPONENT_H__
#define __SPAWNCOMPONENT_H__

#include "cocos2d.h"

class SpawnComponent : public cocos2d::Component
{
public:
	static SpawnComponent* create();

	void onEnter() override;
	
	void BeginSpawn();
	void FinishSpawn();
	void BeginDespawn();
	void FinishDespawn();
	
	void setSpawnAnim(const std::string& animFile,float time,cocos2d::Texture2D* ownerTexture);
	void setDespawnAnim(const std::string& animFile, float time, cocos2d::Texture2D* ownerTexture);

	bool isOwnerSpawning();
	float getSpawnLength();
	cocos2d::Animation* getSpawnAnim();

	bool isOwnerDespawning();
	float getDespawnLength();
	cocos2d::Animation* getDespawnAnim();

CC_CONSTRUCTOR_ACCESS:
	SpawnComponent()
		: super()
		, postSpawnExtraSetup(nullptr)
		, preDespawnExtraSetup(nullptr)
	{
	}

private:
	typedef cocos2d::Component super;

	//any extra setup required before the holding Node becomes active
	std::function<void()> postSpawnExtraSetup;
	//any extra setup required before the holding Node gets destroyed
	std::function<void()> preDespawnExtraSetup;

	bool isSpawning;
	float spawnLength;
	std::string spawnAnimName;

	bool isDespawning;
	float despawnLength;
	std::string despawnAnimName;
};

#endif