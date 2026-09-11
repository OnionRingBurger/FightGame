#include "AI/UseLingeringAttackAction.h"

#include "MathAssist.h"

// !!!New!!!
UseLingeringAttackAction::UseLingeringAttackAction()
{
}

UseLingeringAttackAction::~UseLingeringAttackAction()
{
}

Node::Status UseLingeringAttackAction::Tick(
	AIContext& context,
	const AIBlackboard& blackBord,
	AIMind& mind,
	const AISystemInfo& systemInfo,
	AIResult& result)
{
	(void)mind;
	(void)systemInfo;

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

	const float2 toPlayer(
		blackBord.playerPos.x - enemy.position.x,
		blackBord.playerPos.y - enemy.position.y);
	const float dist = GetLength(toPlayer);
	const float2 nomalizeVec = Normalize(toPlayer);
	const float2 enemyFrontVec = AngleToVector(-enemy.rotation + 90.0f);
	const float dot = nomalizeVec.x * enemyFrontVec.x + nomalizeVec.y * enemyFrontVec.y;
	const float toPlayerAngle = acosf(dot) * DEG;

	int bestAttackIndex = -1;
	float bestDamageValue = -1.0f;

	for (int i = 0; i < static_cast<int>(enemy.attackDatas.size()); i++)
	{
		const AIBlackboard::BBAttackData& attack = enemy.attackDatas.at(static_cast<size_t>(i));

		if (attack.endWithOwnerAction)
		{
			continue;
		}

		if (dist < attack.minLength || dist > attack.maxLength)
		{
			continue;
		}

		const float halfAttackAngle = attack.angle * 0.5f;
		if (halfAttackAngle < toPlayerAngle)
		{
			continue;
		}

		if (attack.damageValue <= bestDamageValue)
		{
			continue;
		}

		bestDamageValue = attack.damageValue;
		bestAttackIndex = i;
	}

	if (bestAttackIndex < 0)
	{
		return NODE_FAILURE;
	}

	result.AttackIndex = bestAttackIndex;
	result.UseAttack = true;
	return NODE_SUCCES;
}
