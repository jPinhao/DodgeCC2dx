#ifndef __MULTICONTROLLER__H_
#define __MULTICONTROLLER__H_

#include "Controller.h"

/*
Multiple entity controller
MultiController can take posession of deveral pawns and manage their updates, initialization, provide player/AI controls
*/
class MultiController : public Controller
{
public:
	enum class MultiControllerUpdate
	{
		//each tick we pick a new Pawn to update sequentially
		SEQUENTIAL_SELECTOR,
		//each tick we pick a new Pawn to update randomly
		RAND_SELECTOR,
		//each tick we update all Pawns
		ALL_PARALLEL
	};

	//controllerUpdateType defines how update() picks the next Pawn to update
	static MultiController* create(MultiControllerUpdate controllerUpdateType = MultiControllerUpdate::RAND_SELECTOR);
	
	//update() should trigger the next update on a/several pawn depedning on the updateType 
	virtual void update(float deltaTime) override;
	//add a new pawn to this manager
	void registerPawn(Pawn* pawn) override;
	//remove a pawn from this manager
	void unregisterPawn(Pawn* pawn) override;
	//return a controlled pawn
	Pawn* getPawn(int pawnIndex) override;
	//is controlling anyone?
	bool isControlling() override;

CC_CONSTRUCTOR_ACCESS:
	//controllerUpdateType defines how update() picks the next Pawn to update
	bool init(MultiControllerUpdate controllerUpdateType);
	MultiController();

	//clear any Pawn/Player tied to the controller - should be called onExit and the destructor
	virtual void clearAllDependants() override;

	//defines how update() picks the next Pawn to update, see MultiControllerUpdate for details
	MultiControllerUpdate updateType;
	//which pawn was last updated
	unsigned int previousUpdatePawn;

	std::vector<Pawn*> allPawns;

private:
	typedef Controller super;
};

#endif