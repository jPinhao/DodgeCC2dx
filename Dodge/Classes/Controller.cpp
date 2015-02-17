#include "Controller.h"
#include "LevelManager.h"

bool Controller::init()
{
	LevelManager *lm = LevelManager::getInstance();
	if (lm->getCurrentLevel())
	{
		lm->getCurrentLevel()->addController(this);
		return true;
	}
}

//enable spawning, this will start spawning if we're managing any spawners
void Controller::enable(bool enable)
{
	if (enabled != enable)
	{
		enabled = enable;
		if (isRunning())
		{
			if (enabled && isControlling()) startController();
			else stopController();
		}
	}
}

void Controller::onEnter()
{
	super::onEnter();
	//start spawning
	if (enabled && isControlling()) startController();
}

void Controller::startController()
{
	scheduleUpdate();
}

void Controller::stopController()
{
	unscheduleUpdate();
}