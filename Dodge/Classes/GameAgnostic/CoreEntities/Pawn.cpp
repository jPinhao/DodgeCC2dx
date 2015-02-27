#include "Pawn.h"
#include "SingleController.h"
#include "PlayerSingleController.h"

USING_NS_CC;

Pawn* Pawn::create(UseController defaultController/* = UseController::AI*/)
{
	return createWithSprite(nullptr);
}

Pawn* Pawn::createWithSprite(Sprite *sprite, UseController defaultController/* = UseController::AI*/)
{
	if (defaultController == UseController::AI) return createWithController(DefaultAIController::create(), sprite);
	else if (defaultController == UseController::PLAYER) return createWithController(DefaultPlayerController::create(nullptr), sprite);
	else return nullptr;
}

Pawn* Pawn::createWithController(Controller* pawnController, Sprite *sprite/* = nullptr*/)
{
	// clone can create without controller CCASSERT(pawnController, "ERROR: Tried to create a Pawn without a controller");
	Pawn *ret = new (std::nothrow) Pawn();
	if (ret != nullptr && ret->initWithController(pawnController, sprite))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

bool Pawn::initWithController(Controller* pawnController, cocos2d::Sprite *sprite)
{
	//setup sprite
	mySprite = sprite;
	if (mySprite)
	{
		addChild(mySprite);
		setupPhysicsBody();
		resizeContentLayer();
	}
	//setup controller
	myController = pawnController;
	posessByController(pawnController);
	return true;
}

void Pawn::resizeContentLayer()
{
	if (mySprite)
	{
		Size spriteSize = Size(mySprite->getContentSize().width*mySprite->getScaleX(), mySprite->getContentSize().height*mySprite->getScaleY());
		Vec2 spriteCentre = Vec2(spriteSize.width, spriteSize.height) / 2;
		//cocos angles are clockwise.... so need to invert for actual angle fo correct cos/sin
		float spriteRotation = CC_DEGREES_TO_RADIANS(-mySprite->getRotation());

		//rotate the size dimensions so we fit the sprite
		//Size volumeSize;
		float absCos = abs(cosf(spriteRotation)), absSin = abs(sinf(spriteRotation));
		Size volumeSize = mySprite->getBoundingBox().size;
		//center the sprite
		mySprite->setPosition(.5f * volumeSize.width, .5f * volumeSize.height);
		//update the Pawn settings to fit the sprite
		setContentSize(volumeSize);
		spriteVertices[0] = Vec2(0.f, 0.f);
		spriteVertices[1] = Vec2(0.f, spriteSize.height);
		spriteVertices[2] = Vec2(spriteSize.width, spriteSize.height);
		spriteVertices[3] = Vec2(spriteSize.width, 0.f);
		//calculate the new vertices based on the rotation (used in Physics shape creation)
		//we don't translate them using the Pawn centre as the physics shapes are positioned relative to the centre
		Vec2 rotateV;
		for (auto& vertex : spriteVertices)
		{
			rotateV = vertex - spriteCentre;
			vertex.x = rotateV.x*cosf(spriteRotation) - rotateV.y*sinf(spriteRotation);
			vertex.y = rotateV.x*sinf(spriteRotation) + rotateV.y*cosf(spriteRotation);
		}
	}
	else
	{
		Size halfSize = getContentSize() / 2;
		spriteVertices[0] = Vec2(-halfSize.width, -halfSize.height);
		spriteVertices[1] = Vec2(-halfSize.width, halfSize.height);
		spriteVertices[2] = Vec2(halfSize.width, halfSize.height);
		spriteVertices[3] = Vec2(halfSize.width, -halfSize.height);
	}
	updatePhysicsBodyShape();
}

void Pawn::setupPhysicsBody()
{
	if (mySprite)
	{ 
		PhysicsBody *pawnBody = PhysicsBody::create();
		pawnBody->setDynamic(true);
		pawnBody->setContactTestBitmask(0xFFFFFFFF);
		setPhysicsBody(pawnBody);
	}
}

//this only supports a single 4 vertex Physics Shape right now - override for custom
void Pawn::updatePhysicsBodyShape()
{
	PhysicsBody *myBody = getPhysicsBody();
	if (myBody)
	{
		myBody->removeAllShapes();
		PhysicsShapePolygon *colPolygon = PhysicsShapePolygon::create(&spriteVertices[0], 4,
			PhysicsMaterial(0.f, 1.f, 0.f));
		colPolygon->setContactTestBitmask(0xFFFFFFFF);
		myBody->addShape(colPolygon, false);
	}
}

void Pawn::postInitializeCustom(void* userData)
{
	return;
}

void Pawn::kill()
{
	removeFromParent();
}

void Pawn::onExit()
{
	super::onExit();
	posessByController(nullptr);
}

Player* Pawn::getControllingPlayer() const
{
	IPlayerController *castController = dynamic_cast<IPlayerController*>(myController);
	if (castController)
	{
		return castController->getOwner();
	}
	return nullptr;
}

//*** Controller functions

//update controlled by the attached Controller - does nothing for base Pawns, should be overriden by subclasses
void Pawn::controlledUpdate()
{
	return;
}

//get the pawn posessed by a specific pre-constructed controller
void Pawn::posessByController(Controller* newController)
{
	if (myController) myController->unregisterPawn(this);
	myController = newController;
	if (myController) myController->registerPawn(this);
}

//create a new default AI controller and get the pawn posessed by it
void Pawn::posessByAIController()
{
	posessByController(DefaultAIController::create());
}
//create a new default Player controller and get the pawn posessed by it
void Pawn::posessByPlayerController()
{
	posessByController(DefaultPlayerController::create(nullptr));
}
//get the Pawn's controller
Controller* Pawn::getController()
{
	return myController;
}


//*** Overriden transform functions

//update scale - acts on Pawn's sprite if one is available
void Pawn::setScaleX(float scaleX)
{
	setScale(scaleX, 1.f);
}
//get scale - returns Pawn's sprite value if one is available
float Pawn::getScaleX() const
{
	if (mySprite)
	{
		return mySprite->getScaleX();
	}
	else return super::getScaleX();
}

//update scale - acts on Pawn's sprite if one is available
void Pawn::setScaleY(float scaleY)
{
	setScale(1.f, scaleY);
}
//get scale - returns Pawn's sprite value if one is available
float Pawn::getScaleY() const
{
	if (mySprite)
	{
		return mySprite->getScaleY();
	}
	else return super::getScaleY();
}

//update scale - acts on Pawn's sprite if one is available
void Pawn::setScale(float scale)
{
	setScale(scale, scale);
}
//update scale - acts on Pawn's sprite if one is available
void Pawn::setScale(float scaleX, float scaleY)
{
	if (mySprite)
	{
		mySprite->setScale(scaleX, scaleY);
	}
	else super::setScale(scaleX, scaleY);

	resizeContentLayer();
}
//get scale - returns Pawn's sprite value if one is available
float Pawn::getScale() const
{
	if (mySprite)
	{
		return mySprite->getScale();
	}
	else return super::getScale();
}

Pawn::Pawn()
	:super(),
	myController(nullptr),
	mySprite(nullptr),
	spriteVertices()
{

}

Pawn::~Pawn()
{
	//make sure we're unregistered
	posessByController(nullptr);
}

Pawn* Pawn::clone() const
{
	Pawn* clonePawn = new (std::nothrow) Pawn(*this);
	if (clonePawn && clonePawn->initWithController(nullptr, clonePawn->mySprite))
	{
		clonePawn->autorelease();
	}
	else CC_SAFE_DELETE(clonePawn);
	return clonePawn;
}

Pawn::Pawn(const Pawn& copy)
	: myController(nullptr)
{
	//clone the sprite
	auto copySprite = copy.mySprite;
	if (copySprite)
	{
		Sprite *cloneSprite = Sprite::createWithTexture(copySprite->getTexture());
		cloneSprite->setScale(copySprite->getScaleX(), copySprite->getScaleY());
		cloneSprite->setRotation(copySprite->getRotation());
		cloneSprite->setPosition(copySprite->getPosition());
		mySprite = cloneSprite;
	}
}