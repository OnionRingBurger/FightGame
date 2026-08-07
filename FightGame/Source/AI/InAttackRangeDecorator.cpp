#include "AI/InAttackRangeDecorator.h"

#include "MathAssist.h"
#include "DebugConsole.h"

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

	// 事前に要素を計算する
	const float2 toPlayer(
		blackBord.playerPos.x - enemy.position.x,
		blackBord.playerPos.y - enemy.position.y);
	const float dist = GetLength(toPlayer);
	float2 nomalizeVec = Normalize(toPlayer);
	float2 enemyFrontVec = AngleToVector(-enemy.rotation + 90.0f);
	float dot = nomalizeVec.x * enemyFrontVec.x + nomalizeVec.y * enemyFrontVec.y;
	float toPlayerAngle = acosf(dot) * DEG;

	// 攻撃範囲を走査し、一つでも範囲内にあれば成功を返す
	for (const AIBlackboard::BBAttackData& attack : enemy.attackDatas)
	{
		float halfAttackAngle = attack.angle * 0.5f;
		DebugConsole::SetDrawPos(1, 10);
		std::cout << "toPlayerAngle: " << toPlayerAngle << ", halfAttackAngle: " << halfAttackAngle << std::endl;

		// 射程距離外だった場合抜ける
		if (!(dist >= attack.minLength) || !(dist <= attack.maxLength))
		{
			continue;
		}

		// 攻撃角より外にいた場合場合抜ける

		if (halfAttackAngle / 5.0f < toPlayerAngle)
		{
			continue;
		}

		// 敵が攻撃範囲内にいるため、関数を終了し成功を返す
		return true;
	}

	// 敵が攻撃範囲内にいなかったため失敗
	return false;
}
