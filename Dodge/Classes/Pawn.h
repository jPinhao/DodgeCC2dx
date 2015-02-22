#ifndef __PAWN__H_
#define __PAWN__H_

#include "cocos2d.h"
#include "Controller.h"

class Controller;
class SingleController;
class PlayerSingleController;
class Player;

/*
Base Pawn class - entities that get posessed by a Contoller (see Controller.h)
--override DefaultAIController/DefaultPlayerController typedef to change default posession for any pawn type
--use createWithController() or posessByController() to use (or change to) a custom controller for a class instance
*/
class Pawn : public cocos2d::Node
{
public:
	enum class UseController
	{
		PLAYER,
		AI
	};

	static Pawn* create(UseController defaultController = UseController::AI);
	static Pawn* createWithSprite(cocos2d::Sprite *sprite, UseController defaultController = UseController::AI);
	static Pawn* createWithController(Controller* pawnController, cocos2d::Sprite *sprite = nullptr);

	//clones an instance of the Pawn and returns it's pointer
	//*a Controller must be attached manually
	virtual Pawn* clone() const;
	//allows custom initialization data to be set through Pawn interface- this applies userData and it's up to caller to clean-up
	virtual void postInitializeCustom(void* userData);

	virtual void kill();
	virtual void onExit();
	//are we controller by a player, which one?
	Player* getControllingPlayer() const;

	//*** Controller functions

	//update controlled by the attached Controller - does nothing for base Pawns, should be overriden by subclasses
	virtual void controlledUpdate();
	//TODO: CHECK THESE WORK WITH TYPEDEF+ NO FUNC OVERRIDE
	//get the pawn posessed by a specific pre-constructed controller
	void posessByController(Controller* newController);
	//create a new default AI controller and get the pawn posessed by it
	void posessByAIController();
	//create a new default Player controller and get the pawn posessed by it
	void posessByPlayerController();
	//get the Pawn's controller
	Controller* getController();

	//*** Overriden transform functions - PhysicsBodies don't support scale (this will eventually take care of that)

	//update scale - acts on Pawn's sprite if one is available
	void setScaleX(float scaleX) override;
	//get scale - returns Pawn's sprite value if one is available
	float getScaleX() const override;

	//update scale - acts on Pawn's sprite if one is available
	void setScaleY(float scaleY) override;
	//get scale - returns Pawn's sprite value if one is available
	float getScaleY() const override;

	//update scale - acts on Pawn's sprite if one is available
	void setScale(float scale) override;
	//update scale - acts on Pawn's sprite if one is available
	void setScale(float scaleX, float scaleY) override;
	//get scale - returns Pawn's sprite value if one is available
	float getScale() const override;

CC_CONSTRUCTOR_ACCESS:
	typedef SingleController DefaultAIController;
	typedef PlayerSingleController DefaultPlayerController;

	Pawn();
	~Pawn();
	static Pawn* create();
	bool initWithController(Controller* pawnController, cocos2d::Sprite *sprite);

	//used by clone() to copy the members
	Pawn(const Pawn& copy);
	void operator=(const Pawn& copy) = delete;

	//update the content layer with new dimensions, taking Sprite into account
	void resizeContentLayer();
	virtual void setupPhysicsBody();
	virtual void updatePhysicsBodyShape();

	Controller *myController;
	cocos2d::Sprite *mySprite;
	cocos2d::Vec2 spriteVertices[4];

private:
	typedef cocos2d::Node super;
};

#endif