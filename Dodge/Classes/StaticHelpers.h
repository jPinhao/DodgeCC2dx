#ifndef __STATICHELPERS__H_
#define __STATICHELPERS__H_

#include "cocos2d.h"

namespace MyHelpers
{
	cocos2d::Node* getPhysShapeOwner(const cocos2d::PhysicsShape* shape);
	void PrepareAnimation(const std::string& framesFileName, float animDuration, cocos2d::Texture2D* originalSprite);
	//transform a Vec2 (transformVector uses Vec3)
	void transformVec2(const cocos2d::Mat4& tranform, cocos2d::Vec2* vecToTransform);
	//transform a Vec2 (transformPoint uses Vec3)
	void transformPoint2(const cocos2d::Mat4& tranform, cocos2d::Vec2* pointToTransform);
}

#endif