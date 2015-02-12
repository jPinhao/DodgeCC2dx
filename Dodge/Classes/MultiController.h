#ifndef __MULTICONTROLLER__H_
#define __MULTICONTROLLER__H_

#include "Controller.h"

class MultiController : public Controller
{
public:
	enum class MultiControllerUpdate
	{
		SEQUENTIAL_SELECTOR,
		RAND_SELECTOR,
		ALL_PARALLEL
	};

	static MultiController* create(MultiControllerUpdate controllerUpdateType = MultiControllerUpdate::RAND_SELECTOR);
	
	//update() ticks down our spawn timer and decides whether it's time to spawn
	virtual void update(float deltaTime) override;
	//add a new spawner to this manager
	void registerPawn(Pawn* pawn) override;
	//remove a spawner from this manager
	void unregisterPawn(Pawn* pawn) override;
	//is controlling anyone?
	bool isControlling() override;

CC_CONSTRUCTOR_ACCESS:
	bool init(MultiControllerUpdate controllerUpdateType);
	MultiController();
	~MultiController();

	MultiControllerUpdate updateType;
	unsigned int previousUpdatePawn;

	std::vector<Pawn*> allPawns;

private:
	typedef Controller super;
};

#endif