#include "AI/UseBestDamageAttackAction.h"

#include "AI/AttackRangeCheck.h"

// !!!New!!!
UseBestDamageAttackAction::UseBestDamageAttackAction()
{
}

UseBestDamageAttackAction::~UseBestDamageAttackAction()
{
}

Node::Status UseBestDamageAttackAction::Tick(
	AIContext& context,
	const AIBlackboard& blackBord,
	AIMind& mind,
	const AISystemInfo& systemInfo,
	AIResult& result)
{
	(void)mind;
	(void)systemInfo;

	// !!!New!!!
	result.UseGuard = false;
	result.MoveDir = float2(0.0f, 0.0f);
	result.IsMove = false;
	result.UseAttack = false;
	result.AttackIndex = 0;

	if (blackBord.isPlayerDead)
	{
		return NODE_FAILURE;
	}

	const auto enemyIt = blackBord.enemyDatas.find(context.agentEntity);
	if (enemyIt == blackBord.enemyDatas.end())
	{
		return NODE_FAILURE;
	}

	const AIBlackboard::BBEnemyData& enemy = enemyIt->second;
	if (enemy.attackDatas.empty())
	{
		return NODE_FAILURE;
	}

	int maxDamageIndex = 0;
	float maxDamageValue = enemy.attackDatas.front().damageValue;

	for (int i = 1; i < static_cast<int>(enemy.attackDatas.size()); i++)
	{
		const AIBlackboard::BBAttackData& attack = enemy.attackDatas.at(static_cast<size_t>(i));
		if (attack.damageValue <= maxDamageValue)
		{
			continue;
		}

		maxDamageValue = attack.damageValue;
		maxDamageIndex = i;
	}

	const AIBlackboard::BBAttackData& maxDamageAttack = enemy.attackDatas.at(static_cast<size_t>(maxDamageIndex));
	if (!IsTargetInAttackRange(blackBord.playerPos, enemy.position, enemy.rotation, maxDamageAttack))
	{
		return NODE_FAILURE;
	}

	result.AttackIndex = maxDamageIndex;
	result.UseAttack = true;
	return NODE_SUCCES;
}
