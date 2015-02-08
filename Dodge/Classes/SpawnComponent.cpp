#include "SpawnComponent.h"
#include "AnimationLoader.h"

USING_NS_CC;

SpawnComponent* SpawnComponent::create()
{
	SpawnComponent * ret = new (std::nothrow) SpawnComponent();
	if (ret != nullptr && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

void SpawnComponent::onEnter()
{
	super::onEnter();
	BeginSpawn();
}

void SpawnComponent::BeginSpawn()
{
	Vector<FiniteTimeAction*> spawnSequence;

	bool hasSpawnAnim = !spawnAnimName.empty();
	//spawning anim action added top sequence
	if (hasSpawnAnim)
	{
		//set animate action, queue FinishSpawn()
		Animation *spawnAnim = AnimationCache::getInstance()->getAnimation(spawnAnimName);
		if (spawnAnim)
		{
			spawnSequence.pushBack(Animate::create(spawnAnim));
		}
	}

	//postspawn sequence, any extra setup is done before we activate the node
	if (!postSpawnExtraSetup._Empty())
	{
		spawnSequence.pushBack(CallFunc::create(postSpawnExtraSetup));
	}
	spawnSequence.pushBack(CallFunc::create(CC_CALLBACK_0(SpawnComponent::FinishSpawn, this)));

	Node *owner = getOwner();
	owner->runAction(Sequence::create(spawnSequence));
	//disable any physics and updates until we're done
	if (owner->getPhysicsBody()) owner->getPhysicsBody()->setEnable(false);
	owner->unscheduleUpdate();

	//flag us as spawning
	isSpawning = true;
}

void SpawnComponent::FinishSpawn()
{
	Node *owner = getOwner();
	if (owner->getPhysicsBody()) owner->getPhysicsBody()->setEnable(true);
	owner->scheduleUpdate();

	//flag us as done spawning
	isSpawning = false;
}

void SpawnComponent::BeginDespawn()
{
	Vector<FiniteTimeAction*> despawnSequence;

	//postspawn sequence, runs after spawn anim and ends with holding Node being activated
	if (!preDespawnExtraSetup._Empty())
	{
		despawnSequence.pushBack(CallFunc::create(postSpawnExtraSetup));
	}

	bool hasDepawnAnim = despawnAnimName.empty();
	//despawning anim action added to sequence
	if (hasDepawnAnim)
	{
		//set animate action, queue FinishSpawn()
		Animation *despawnAnim = AnimationCache::getInstance()->getAnimation(despawnAnimName);
		if (despawnAnim)
		{
			despawnSequence.pushBack(Animate::create(despawnAnim));
		}
	}
	//and finish despawn
	despawnSequence.pushBack(CallFunc::create(CC_CALLBACK_0(SpawnComponent::FinishDespawn, this)));

	Node *owner = getOwner();
	//disable any physics and updates until we're done
	if (owner->getPhysicsBody()) owner->getPhysicsBody()->setEnable(false);
	owner->unscheduleUpdate();
	owner->runAction(Sequence::create(despawnSequence));

	//flag us as despawning
	isDespawning = true;
}

void SpawnComponent::FinishDespawn()
{
	Node *owner = getOwner();
	owner->removeFromParent();
	//flag us as despawning
	isDespawning = false;
}

void SpawnComponent::setSpawnAnim(const std::string& animFile, float animDuration, Texture2D* ownerTexture)
{
	spawnAnimName = animFile;
	spawnLength = MAX(0, animDuration);
	AnimationLoader::PrepareAnimation(animFile, spawnLength, ownerTexture);
}

void SpawnComponent::setDespawnAnim(const std::string& animFile, float animDuration, Texture2D* ownerTexture)
{
	despawnAnimName = animFile;
	despawnLength = MAX(0, animDuration);
	AnimationLoader::PrepareAnimation(animFile, despawnLength, ownerTexture);
}

bool SpawnComponent::isOwnerSpawning()
{ 
	return isSpawning; 
}

float SpawnComponent::getSpawnLength()
{
	return spawnLength;
}

Animation* SpawnComponent::getSpawnAnim()
{
	return AnimationCache::getInstance()->getAnimation(spawnAnimName);
}

bool SpawnComponent::isOwnerDespawning()
{
	return isDespawning;
}

float SpawnComponent::getDespawnLength()
{
	return despawnLength;
}

Animation* SpawnComponent::getDespawnAnim()
{
	return AnimationCache::getInstance()->getAnimation(despawnAnimName);
}