#ifndef __ANIMATIONLOADER_H__
#define __ANIMATIONLOADER_H__

#include "cocos2d.h"

class AnimationLoader
{
public:
	static void PrepareAnimation(const std::string& framesFileName, float animDuration, cocos2d::Texture2D* originalSprite);
};

#endif