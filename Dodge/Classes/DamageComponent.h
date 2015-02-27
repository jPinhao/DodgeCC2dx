#ifndef __PAWNDAMAGECOMPONENT_H__
#define __PAWNDAMAGECOMPONENT_H__

#include "cocos2d.h"
#include "EntityComponents\UpdatingComponent.h"
#include "CoreEntities\Pawn.h"

#define DMG_PLAYER_OTHER 0x000F
#define DMG_AI_OTHER 0x00F0
#define DMG_SELF 0x0F00
#define DMG_SCENERY 0xF000

//class Node;

typedef short damageMask;

//maintains health and ability to be damaged
class TakesDamageComponent : public UpdatingComponent
{
public:
	static TakesDamageComponent* create(float maximumLife);

	//update applies the damage
	void update(float deltaTime) override;

	//adds more damage to be applied
	//actually applying the damage is done on the next update cycle
	void addDamage(float damage, damageMask damageType);

	float getMaxLife();
	float getCurrentLife();
	float getPendingDamage();

	//delegate to be called to apply custom behaviour when Life does change
	std::function<void(float)> onLifeUpdate;

CC_CONSTRUCTOR_ACCESS:
	bool init(float maximumLife);
	
	TakesDamageComponent()
		: super()
	{
	}

private:
	typedef UpdatingComponent super;

	float maxLife;
	float life;
	float damageToApply;
};

//adds an aura to the Node that does damage to anything it connects with
class AuraDamageComponent : public cocos2d::Component
{
public:
	static AuraDamageComponent* create(float radius, float minDmg, float maxDmg, damageMask validTargets, damageMask damageType);

	void onEnter() override;
	void onExit() override;

	//try to apply damage to the target Node
	void doDamage(cocos2d::Node& target, float dmgMultiplier);

CC_CONSTRUCTOR_ACCESS:
	bool init(float radius, float minDmg, float maxDmg, damageMask validTargets, damageMask damageType);
	
	AuraDamageComponent()
		: super()
	{
	}

private:
	typedef cocos2d::Component super;

	bool canDamage(cocos2d::Node* target);

	float myRadius;
	float min;
	float max;
	damageMask targets;
	damageMask type;
};

#endif