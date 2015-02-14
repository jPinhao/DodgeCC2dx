#ifndef __CONTROLLER__H_
#define __CONTROLLER__H_

#include "cocos2d.h"
#include "Pawn.h"

class Pawn;

/* 
Controller base class - shouldn't be instantiated
Controllers can take posession of a/multiple pawns and manage their updates, initialization, provide player/AI controls
*/
class Controller : public cocos2d::Node
{
public:
	//try to start the controller running (if we're enabled and are have a pawn to control)
	virtual void onEnter() override;

	//All these functions need to be implemented but vary greatly depending on Single/Multi controller
	//update() should trigger the next update on the pawn(s) 
	virtual void update(float deltaTime)=0;
	//add a new Pawn to this controller
	virtual void registerPawn(Pawn* myPawn)=0;
	//remove a Pawn from this controller
	virtual void unregisterPawn(Pawn* myPawn)=0;
	//is controlling anyone?
	virtual bool isControlling()=0;

	//enable spawning, this will start spawning if we're managing any spawners
	void enable(bool enable);

CC_CONSTRUCTOR_ACCESS:
	Controller() : super(){};

	//start the updates
	virtual void startController();
	//stop the updates
	virtual void stopController();

	bool enabled;

private:
	typedef cocos2d::Node super;

	//prevent Controller creation
	static void create() = delete;
	Controller(const Controller&) = delete;
	void operator=(const Controller&) = delete;
};

#endif