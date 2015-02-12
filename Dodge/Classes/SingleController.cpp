#include "SingleController.h"

SingleController* SingleController::create()
{
	SingleController *ret = new(std::nothrow) SingleController();
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool SingleController::init()
{
	return true;
}

//TODO - pawns registered with otehr controllers?

//add a new spawner to this manager
void SingleController::registerPawn(Pawn* pawn)
{
	if (!pawn) return;

	if (!myPawn) startController();
	myPawn = pawn;
	myPawn->retain();
}

//remove a spawner from this manager
void SingleController::unregisterPawn(Pawn* pawn)
{
	if (!pawn) return;

	if (myPawn && myPawn == pawn)
	{
		myPawn->release();
		stopController();
	}
}

void SingleController::update(float deltaTime)
{
	//double check we're supposed to be running
	if (enabled && isControlling())
	{
		if (myPawn) myPawn->controlledUpdate();
	}
	else stopController();
}

bool SingleController::isControlling()
{
	if(myPawn) return true;
	else return false;
}

SingleController::SingleController()
	:super()
	, myPawn(nullptr)
{
}

SingleController::~SingleController()
{
	if (myPawn) myPawn->release();
	super::~super();
}