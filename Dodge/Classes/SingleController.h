#ifndef __SINGLECONTROLLER__H_
#define __SINGLECONTROLLER__H_

#include "cocos2d.h"
#include "Controller.h"

/*
Single entity controller
SingleController can take posession of a pawn and manage its updates, initialization, provide player/AI controls
*/
class SingleController : public Controller
{
public:
	static SingleController* create();

	//update() should trigger the next update on the pawn(s
	virtual void update(float deltaTime) override;
	//change the Pawn we're controlliing
	void registerPawn(Pawn* myPawn) override;
	//stop controlling this pawn
	void unregisterPawn(Pawn* myPawn) override;
	Pawn* getPawn();
	//is controlling anyone?
	bool isControlling() override;

CC_CONSTRUCTOR_ACCESS:
	SingleController();
	//clear any Pawn/Player tied to the controller - should be called onExit and the destructor
	virtual void clearAllDependants() override;

	//who are we controlling
	Pawn *myPawn;

private:
	typedef Controller super;
};

#endif