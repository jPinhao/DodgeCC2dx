#ifndef __PAWN__H_
#define __PAWN__H_

#include "cocos2d.h"
#include "Controller.h"

class Controller;

class Pawn : public cocos2d::Node
{
public:
	static Pawn* create();
	virtual void controlledUpdate(){ return; };

CC_CONSTRUCTOR_ACCESS:
	bool init(){return true;};
	Pawn() :super(), mySprite(nullptr) {};
	~Pawn(){ super::~super(); };

	Controller *myController;

private:
	typedef cocos2d::Node super;

	cocos2d::Sprite *mySprite;
};

#endif