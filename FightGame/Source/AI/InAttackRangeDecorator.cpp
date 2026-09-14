#include "AI/InAttackRangeDecorator.h"

#include "AI/AttackRangeCheck.h"

// !!!New!!!
InAttackRangeDecorator::InAttackRangeDecorator(std::unique_ptr<Node> child)
	: DecoratorNode(std::move(child))
{
}

InAttackRangeDecorator::~InAttackRangeDecorator()
{
}

bool InAttackRangeDecorator::CheckCondition(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	(void)mind;
	(void)systemInfo;
	(void)result;

	// TODO 恐らく範囲外でも通してしまうバグがある模様、あるいは下のAttack側がおかしいかも

	// プレイヤーが死んでいた場合抜ける
	if (blackBord.isPlayerDead)
	{
		return false;
	}

	// 自身のBB情報を取得する
	const auto enemyIt = blackBord.enemyDatas.find(context.agentEntity);
	if (enemyIt == blackBord.enemyDatas.end())
	{
		return false;
	}

	const AIBlackboard::BBEnemyData& enemy = enemyIt->second;
	if (enemy.attackDatas.empty())
	{
		return false;
	}

	// 攻撃範囲を走査し、一つでも範囲内にあれば成功を返す
	for (const AIBlackboard::BBAttackData& attack : enemy.attackDatas)
	{
		if (IsTargetInAttackRange(blackBord.playerPos, enemy.position, enemy.rotation, attack))
		{
			// 敵が攻撃範囲内にいるため、関数を終了し成功を返す
			return true;
		}
	}

	// 敵が攻撃範囲内にいなかったため失敗
	return false;
}
