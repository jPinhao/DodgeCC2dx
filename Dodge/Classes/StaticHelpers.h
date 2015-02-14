#ifndef __STATICHELPERS__H_
#define __STATICHELPERS__H_

#include "cocos2d.h"

namespace MyHelpers
{
	cocos2d::Node* getPhysShapeOwner(const cocos2d::PhysicsShape* shape);
	void PrepareAnimation(const std::string& framesFileName, float animDuration, cocos2d::Texture2D* originalSprite);
}

#endif