#ifndef __PELLET_H__
#define __PELLET_H__

#include "cocos2d.h"
#include "Pawn.h"

#define PELLET_SPEED 300.f
#define PELLET_NOTARGETX -99999
#define PELLET_NOTARGETY PELLET_NOTARGETX

class DodgePlayerController;

class Pellet : public Pawn
{
public:
	static Pellet* create(UseController defaultController = UseController::AI);
	static Pellet* createWithController(Controller *pawnController);

	static const Pellet& getDefaultObject();
	//clones an instance of the Pellet and returns it's pointer
	//*a Controller must be attached manually
	//*moveDirection needs to be set manually 
	Pellet* clone() const;

	void onEnter() override;
	void kill() override;

	//should receive a Vec2 through, use to set initial move direction
	void postInitializeCustom(void* userData) override;

	// frame update 
	void update(float deltaTime) override;

	// check if this is a player vs other pellet to kill the player and ignore collision
	bool onContactBegin(cocos2d::PhysicsContact &contact);
	// ensure pellet exits collision with the correct speed
	void onContactPostSolve(cocos2d::PhysicsContact& contact, const cocos2d::PhysicsContactPostSolve& solve);

	//bool IsSpawning();

	void setTargetPosition(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void clearTargetPosition(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void SetMovementDirection(cocos2d::Vec2 newDirection);
	//FVector& GetMovementDirection();

CC_CONSTRUCTOR_ACCESS:
	bool initWithController(Controller* pawnController, cocos2d::Sprite *sprite);

	//used by clone() to copy the members
	Pellet();
	Pellet(const Pellet& copy);
	void operator=(const Pellet& copy) = delete;

	void setupPhysicsBody() override;
	void updatePhysicsBodyShape() override;

private:
	typedef Pawn super;
	typedef DodgePlayerController DefaultPlayerController;

	float moveSpeed = PELLET_SPEED;
	cocos2d::Vec2 touchPosition;
	cocos2d::Vec2 moveDirection;

	//object utilities
	//FVector CalcBounceDirection(const FVector &myDirection, const FVector &hitNormal);
	//UPrimitiveComponent *colisionComponent;
};

#endif