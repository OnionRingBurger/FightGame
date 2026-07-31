#pragma once
#include <unordered_map>
#include <vector>
#include <string>

#include "MathAssist.h"
#include "ECSTypes.h"

// !!!New!!!
struct AIBlackboard
{
	struct BBAttackData
	{
		// 攻撃範囲
		float minLength;
		float maxLength;
		float angle;
		// 威力、隙
		float damageValue;
		float waitTime;
	};

	struct BBEnemyData
	{
		// !!!New!!!
		float2 position;
		float rotation;

		// 行動フラグ
		bool canMove;
		bool canAttack;

		// 攻撃データ
		BBAttackData attackData;
	};

	float2 playerPos;
	bool isPlayerDead;

	std::unordered_map<Entity, BBEnemyData> enemyDatas;
};

// !!!New!!!
struct AISenseFrame
{
	float2 playerPos;
	bool isPlayerDead;
	std::unordered_map<Entity, AIBlackboard::BBEnemyData> enemyDatas;
};

// !!!New!!!
struct AIContext
{
	std::string BTType;
	Entity agentEntity;
	std::vector<int> nodeIndexes;
	float actionDuration;
};

// !!!New!!!
struct AIResult
{
	float2 MoveDir;
	bool UseAttack;
	bool IsMove;
};

struct AIRuntime
{
	AIContext context;
	AIResult result;
};