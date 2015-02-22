#ifndef __NOTIFIERCOMPONENT_H__
#define __NOTIFIERCOMPONENT_H__

#define EVENT_PELLET_SPAWN "Pel_Sp"
#define EVENT_PELLET_DIE "Pel_Die"
#define EVENT_PLAYER_SCORE_CHANGE "Pl_Score"

// Event that get's sent when a Pawn dies
//** retains sourcePawn to ensure it is still accessible by the time the event get's sent
class PawnDeathEvent : public cocos2d::EventCustom
{
public:
	PawnDeathEvent(const std::string& eventName, Pawn& sourcePawn)
		: super(eventName)
	{
		this->sourcePawn = &sourcePawn;
		this->sourcePawn->retain();
	}
	~PawnDeathEvent()
	{
		sourcePawn->release();
	}
	Pawn* getPawn() { return sourcePawn; };

private:
	typedef cocos2d::EventCustom super;
	Pawn* sourcePawn;
};

#endif