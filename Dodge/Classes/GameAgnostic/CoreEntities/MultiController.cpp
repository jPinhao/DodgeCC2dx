#include "MultiController.h"
#include "base\ccRandom.h"

MultiController* MultiController::create(MultiControllerUpdate controllerUpdateType/*= MultiControllerUpdate::RAND_SELECTOR*/)
{
	MultiController *ret = new(std::nothrow) MultiController();
	if (ret && ret->init(controllerUpdateType))
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

bool MultiController::init(MultiControllerUpdate controllerUpdateType)
{
	super::init();
	updateType = controllerUpdateType;
	return true;
}

void MultiController::update(float deltaTime)
{
	//double check we're supposed to be running
	if (enabled && isControlling())
	{
		switch (updateType)
		{
			case MultiControllerUpdate::ALL_PARALLEL:
			{
				for (auto pawn : allPawns)
				{
					pawn->controlledUpdate();
				}
				break;
			}
			case MultiControllerUpdate::RAND_SELECTOR:
			{
				short randomPawn = cocos2d::RandomHelper::random_int(0, (int)allPawns.size() - 1);
				Pawn *pawn = allPawns[randomPawn];
				if (pawn) pawn->controlledUpdate();
				break;
			}
			case MultiControllerUpdate::SEQUENTIAL_SELECTOR:
			{
				previousUpdatePawn++;
				if (previousUpdatePawn >= allPawns.size()) previousUpdatePawn = 0;
				Pawn *pawn = allPawns[previousUpdatePawn];
				if (pawn) pawn->controlledUpdate();
				break;
			}
		}
	}
	else stopController();
}

void MultiController::registerPawn(Pawn* pawn)
{
	if (!pawn) return;
	//check if this spawner has already been registered
	if (std::find(allPawns.begin(), allPawns.end(), pawn) != allPawns.end())
	{
		return;
	}

	allPawns.push_back(pawn);
	pawn->retain();
	//start spawning, we've got someone to manage!
	if (enabled && allPawns.size() == 1) startController();
}
void MultiController::unregisterPawn(Pawn* pawn)
{
	if (!pawn) return;
	std::vector<Pawn*>::iterator new_end = std::remove(allPawns.begin(), allPawns.end(), pawn);
	//double check we do contain the spawner so we don't release a reference we don't hold
	if (new_end != allPawns.end())
	{
		allPawns.erase(new_end, allPawns.end());
		pawn->release();
		//check if we need to bring the previousUpdatePawn index down so we don't skip an update
		if (updateType == MultiControllerUpdate::SEQUENTIAL_SELECTOR)
		{
			if (new_end - allPawns.begin() < previousUpdatePawn) previousUpdatePawn--;
		}
	}
	//disable spawning, we're not managing anyone
	if (!isControlling()) stopController();
}

Pawn* MultiController::getPawn(int pawnIndex)
{
	if (pawnIndex<0 || pawnIndex >= allPawns.size()) return nullptr;
	else return allPawns[pawnIndex];
}

bool MultiController::isControlling()
{
	return !allPawns.empty();
}

//clear any Pawn/Player tied to the controller - should be called onExit and the destructor
void MultiController::clearAllDependants()
{
	for (auto pawn : allPawns)
	{
		pawn->release();
	}
	allPawns.clear();
}

MultiController::MultiController()
	: super()
	, previousUpdatePawn(0)
{
}