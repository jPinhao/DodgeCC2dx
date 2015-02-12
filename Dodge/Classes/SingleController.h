#ifndef __SINGLECONTROLLER__H_
#define __SINGLECONTROLLER__H_

#include "cocos2d.h"
#include "Controller.h"

class SingleController : public Controller
{
public:
	static SingleController* create();
	
	//update() ticks down our spawn timer and decides whether it's time to spawn
	virtual void update(float deltaTime) override;
	//add a new spawner to this manager
	void registerPawn(Pawn* myPawn) override;
	//remove a spawner from this manager
	void unregisterPawn(Pawn* myPawn) override;
	//is controlling anyone?
	bool isControlling() override;

CC_CONSTRUCTOR_ACCESS:
	bool init();
	SingleController();
	~SingleController();

	Pawn *myPawn;

private:
	typedef Controller super;
};

#endif