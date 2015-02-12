#ifndef __CONTROLLER__H_
#define __CONTROLLER__H_

#include "cocos2d.h"
#include "Pawn.h"

class Pawn;

class Controller : public cocos2d::Node
{
public:
	//try to start spawning (if we're enabled and have registered spawners)
	virtual void onEnter() override;

	//update() ticks down our spawn timer and decides whether it's time to spawn
	virtual void update(float deltaTime)=0;
	//add a new spawner to this manager
	virtual void registerPawn(Pawn* myPawn)=0;
	//remove a spawner from this manager
	virtual void unregisterPawn(Pawn* myPawn)=0;
	//is controlling anyone?
	virtual bool isControlling()=0;

	//enable spawning, this will start spawning if we're managing any spawners
	void enable(bool enable);

CC_CONSTRUCTOR_ACCESS:
	Controller():super(){};
	~Controller(){ super::~super(); };

	//start/stop the updates
	virtual void startController();
	virtual void stopController();

	Pawn *myPawn;
	bool enabled;

private:
	typedef cocos2d::Node super;
};

#endif