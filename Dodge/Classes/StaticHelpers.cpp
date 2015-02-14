#include "StaticHelpers.h"

USING_NS_CC;

//find who owns a given Physics Shape
Node* MyHelpers::getPhysShapeOwner(const PhysicsShape* shape)
{
	Node* retNode = nullptr;
	PhysicsBody* physBody = nullptr;
	if (shape)
	{
		physBody = shape->getBody();
		if (physBody)
		{
			retNode = physBody->getNode();
		}
	}
	return retNode;
}

//generate and cache an anim
void MyHelpers::PrepareAnimation(const std::string& framesFileName, float animDuration, Texture2D* originalSprite)
{
	AnimationCache *cache = AnimationCache::getInstance();
	//check if we have the anim already cached
	if (!cache->getAnimation(framesFileName))
	{
		Vector<SpriteFrame*> animationFrames;
		Size frameSize = originalSprite->getContentSizeInPixels();
		Texture2D *framesTexture = Director::getInstance()->getTextureCache()->addImage(framesFileName);
		if (framesTexture)
		{
			for (float frameY = framesTexture->getContentSizeInPixels().height; frameY > 0; frameY -= frameSize.height)
			{
				for (float frameX = framesTexture->getContentSizeInPixels().width; frameX > 0; frameX -= frameSize.width)
				{
					SpriteFrame *nextFrame = SpriteFrame::createWithTexture(framesTexture,
						Rect(frameX - frameSize.width, frameY - frameSize.height, frameSize.width, frameSize.height),
						false, Vec2::ZERO, frameSize);
					if (nextFrame) animationFrames.pushBack(nextFrame);
				}
			}
		}

		CCASSERT(animationFrames.size() > 0, "ERROR: PrepareAnimation failed to generate any animation frames");
		if (animationFrames.size() != 0)
		{
			Animation *newAnim = Animation::createWithSpriteFrames(animationFrames);
			if (animDuration>0) newAnim->setDelayPerUnit(animDuration / (float)animationFrames.size());
			cache->addAnimation(newAnim, framesFileName);
		}
	}
}