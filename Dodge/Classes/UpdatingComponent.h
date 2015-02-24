#ifndef __UPDATINGCOMPONENT_H__
#define __UPDATINGCOMPONENT_H__

#include "cocos2d.h"

//component that actually has it's update() function scheduled
class UpdatingComponent : public cocos2d::Component
{
	virtual void onEnter() override
	{
		super::onEnter();
		_owner->getScheduler()->schedule(schedule_selector(UpdatingComponent::update), this, 0.0f, false);
	};

	virtual void onExit() override
	{
		super::onExit();
		_owner->getScheduler()->unschedule(schedule_selector(UpdatingComponent::update), this);
	};

CC_CONSTRUCTOR_ACCESS:
	UpdatingComponent()
		: super()
	{
	}

private:
	typedef cocos2d::Component super;
};


#endif