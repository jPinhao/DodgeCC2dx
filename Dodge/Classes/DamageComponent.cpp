#include "DamageComponent.h"
#include "StaticHelpers.h"
#include "object_tags.h"

USING_NS_CC;

TakesDamageComponent* TakesDamageComponent::create(float maximumLife)
{
	TakesDamageComponent * ret = new (std::nothrow) TakesDamageComponent();
	if (ret != nullptr && ret->init(maximumLife))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

bool TakesDamageComponent::init(float maximumLife)
{
	if (super::init())
	{
		setName("TakesDamageComponent");
		maxLife = maximumLife;
		life = maxLife;
		damageToApply = 0;
		return true;
	}
	return false;
}

//update applies the damage
void TakesDamageComponent::update(float deltaTime)
{
	if (damageToApply > 0)
	{
		float previousLife = life;
		if (damageToApply > life) life = 0;
		else life -= damageToApply;
		damageToApply = 0;

		Pawn *owningPawn = dynamic_cast<Pawn*>(_owner);
		if (owningPawn)
		{
			if (life == 0)
			{
				//we're being removed after the kill() call, don't do anything after
				owningPawn->kill();
			}
			else if (onLifeUpdate != nullptr) onLifeUpdate(damageToApply);
		}
	}
}

void TakesDamageComponent::addDamage(float damage, damageMask damageType)
{
	if (isEnabled() && _owner->isRunning())
	{
		damageToApply += damage;
	}
}

float TakesDamageComponent::getMaxLife()
{
	return maxLife;
}

float TakesDamageComponent::getCurrentLife()
{
	return life;
}

float TakesDamageComponent::getPendingDamage()
{
	return damageToApply;
}




AuraDamageComponent* AuraDamageComponent::create(float radius, float minDmg, float maxDmg, damageMask validTargets, damageMask damageType)
{
	AuraDamageComponent * ret = new (std::nothrow) AuraDamageComponent();
	if (ret != nullptr && ret->init(radius, minDmg, maxDmg, validTargets, damageType))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

bool AuraDamageComponent::init(float radius, float minDmg, float maxDmg, damageMask validTargets, damageMask damageType)
{
	if (super::init())
	{
		setName("AuraDamageComponent");
		myRadius = radius;
		min = minDmg;
		max = maxDmg;
		targets = validTargets;
		type = damageType;
		return true;
	}
	return false;
}

void AuraDamageComponent::onEnter() 
{
	super::onEnter();
	//add the aura Sprite
	auto auraSprite = Sprite::create("halo.png");
	auraSprite->setScale(myRadius / (auraSprite->getContentSize().width / 2));
	auraSprite->setPosition(_owner->getContentSize() / 2);
	auraSprite->setTag(TAG_GAME_COMNODE_AURADMG_SPRITE);
	_owner->addChild(auraSprite, -1);

	//add the aura Shape that will respond to contacts
	if (!_owner->getPhysicsBody()) _owner->setPhysicsBody(PhysicsBody::create());
	auto aura = PhysicsShapeCircle::create(myRadius, PhysicsMaterial(0, 0, 0));
	aura->setContactTestBitmask(0xFFFFFFFF);
	aura->setTag(TAG_GAME_COMNODE_AURADMG_SHAPE);
	_owner->getPhysicsBody()->addShape(aura);
	//setup the contact listener
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactPreSolve = [this, aura](PhysicsContact& contact, PhysicsContactPreSolve& solve){
			Node *otherActor = nullptr;
			//see if this is our collision, and find the other Node
			if (aura == contact.getShapeA()) otherActor = MyHelpers::getPhysShapeOwner(contact.getShapeB());
			else if (aura == contact.getShapeB()) otherActor = MyHelpers::getPhysShapeOwner(contact.getShapeA());
			else return true;

			//try to apply damage
			this->doDamage(*otherActor, Director::getInstance()->getDeltaTime());
			return false;
	};
	_owner->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, _owner);
}

void AuraDamageComponent::onExit()
{
	auto auraSprite = _owner->getChildByTag(TAG_GAME_COMNODE_AURADMG_SPRITE);
	if (auraSprite) auraSprite->removeFromParent();
	if (_owner->getPhysicsBody())
	{
		auto auraShape = _owner->getPhysicsBody()->getShape(TAG_GAME_COMNODE_AURADMG_SHAPE);
		if (auraShape) _owner->getPhysicsBody()->removeAllShapes(auraShape);
	}
}

void AuraDamageComponent::doDamage(Node& target, float dmgMultiplier)
{
	Component *targetCom = target.getComponent("TakesDamageComponent");
	if (targetCom)
	{
		if (canDamage(&target))
		{
			TakesDamageComponent *takeDmgCom = static_cast<TakesDamageComponent*>(targetCom);
			float damage = RandomHelper::random_real(min, max);
			takeDmgCom->addDamage(damage*dmgMultiplier, type);
		}
	}
}

bool AuraDamageComponent::canDamage(Node* target)
{
	Pawn* targetPawn = dynamic_cast<Pawn*>(target);
	if (target == _owner)
	{
		return(targets & DMG_SELF);
	}
	else if (!targetPawn)
	{
		//not a pawn, must be a static world element
		return(targets & DMG_SCENERY);
	}
	else
	{
		//check if we can do damage against target type
		if (targetPawn->getControllingPlayer())
		{
			return(targets & DMG_PLAYER_OTHER);
		}
		else
		{
			return(targets & DMG_AI_OTHER);
		}
	}
}