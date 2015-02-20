#ifndef __PLAYERSTATE__H_
#define __PLAYERSTATE__H_

#include "PlayerVariable.h"
#include <vector>
#include <memory>

struct State
{
public:
	bool setVariableByID(unsigned int varID, int val);
	int getVariableByID(unsigned int varID);
protected:
	std::vector< std::unique_ptr<PlayerVariable<void*>> > stateVariables;
};

struct AccountState : public State
{
	AccountState(unsigned int player)
		:playerID(player)
	{
		auto v1 = new PlayerVariable<unsigned int>(VAR_PREMIUM_CURRENCY, true);
		auto v2 = new PlayerVariable<unsigned int>(VAR_RESPAWNS, true);
		auto v3 = new PlayerVariable<unsigned int>(VAR_MULTIPLIERS, true);
		auto v4 = new PlayerVariable<unsigned int>(VAR_TOTAL_SPAWNED, false);
		auto v5 = new PlayerVariable<unsigned int>(VAR_TOTAL_KILLED, false);
		auto v6 = new PlayerVariable<unsigned int>(VAR_MOST_AT_ONCE, false);
		auto v7 = new PlayerVariable<unsigned int>(VAR_MOST_KILLED, false);
		
		stateVariables =
		{
			//currency
			std::unique_ptr<PlayerVariable<unsigned int>>(v1),
			std::unique_ptr<PlayerVariable<unsigned int>>(v2),
			std::unique_ptr<PlayerVariable<unsigned int>>(v3),
			std::unique_ptr<PlayerVariable<unsigned int>>(v4),
			std::unique_ptr<PlayerVariable<unsigned int>>(v5),
			std::unique_ptr<PlayerVariable<unsigned int>>(v6),
			std::unique_ptr<PlayerVariable<unsigned int>>(v7)
		};
	}
	unsigned int playerID;
	//level highscores
	//std::vector<LevelScoreState> levelHighscores;
	//achievements
	//std::vector<Achievements> playerAchievements;
};

struct PlayerSessionState : public State
{
	AccountState playerAccount;
	LevelScoreState currentLevelState;
	PlayerController* myController;
};

struct LevelScoreState : public State
{
public:
	LevelScoreState(unsigned int level)
		:levelID(level),
		unlocked(false)
	{
		stateVariables =
		{
			PlayerVariable(VAR_TOTAL_SPAWNED, false),
			PlayerVariable(VAR_TOTAL_KILLED, false),
			PlayerVariable(VAR_MOST_AT_ONCE, false)
		};
	}
	bool isUnlocked();
	void unlock();

private:
	unsigned int levelID;
	bool unlocked : 1;
};

#endif