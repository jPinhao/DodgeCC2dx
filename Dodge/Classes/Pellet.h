#ifndef __PELLET_H__
#define __PELLET_H__

#include "cocos2d.h"

#define PELLET_SPEED 500.f
#define PELLET_NOTARGETX -99999
#define PELLET_NOTARGETY PELLET_NOTARGETX

class Pellet : public cocos2d::Sprite
{
public:
	static Pellet* create();

	/* after all game elements are created */
	//virtual void PostInitializeComponents() override;
	//virtual void BeginPlay();

	// frame update 
	void update(float deltaTime) override;

	// OnColide event
	//bool OnContactBegin(cocos2d::PhysicsContact &contact);

	//void ChaseTarget(float deltaTime);
	//bool CanChase();
	//bool IsSpawning();

	void setTargetPosition(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void clearTargetPosition(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	//bool SetMovementDirection(const FVector &movementDirection);
	//FVector& GetMovementDirection();

CC_CONSTRUCTOR_ACCESS:
	bool initWithFile(const std::string& filename);

private:
	typedef cocos2d::Sprite super;

	float moveSpeed = PELLET_SPEED;
	cocos2d::Vec2 touchPosition;
	//spawning
	//UPaperFlipbook *spawnAnimation;
	void onEnter() override;
	void BeginSpawn();
	void FinishSpawn();

	//animation utilities
	//void PrepareAnimation(UPaperFlipbook *animation, bool loop);
	//void FinishAnimation();
	//UPaperFlipbookComponent *animComponent;
	//UPaperSpriteComponent *spriteComponent;


	//object utilities
	//FVector CalcBounceDirection(const FVector &myDirection, const FVector &hitNormal);
	//UPrimitiveComponent *colisionComponent;
};

#endif