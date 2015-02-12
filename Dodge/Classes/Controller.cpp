#include "Controller.h"


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