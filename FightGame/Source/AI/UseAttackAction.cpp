#include "AI/UseAttackAction.h"

#include "AI/AttackRangeCheck.h"

// !!!New!!!
UseAttackAction::UseAttackAction()
{
}

UseAttackAction::~UseAttackAction()
{
}

Node::Status UseAttackAction::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	(void)mind;
	(void)systemInfo;

	result.UseGuard = false;

		// 自身のBB情報を取得する
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
	
		// TODO 本来はユーティリティAIが上層でベストな攻撃を選択するが、仮設的にこちらで攻撃を決定する
		int attackIndex = 0;
		// プレイヤーが死んでいた場合抜ける
		if (blackBord.isPlayerDead)
		{
			return NODE_FAILURE;
		}

		if(enemy.attackDatas.empty())
		{
			return NODE_FAILURE;
		}

		
		bool isInAttackRange = false;

		// 攻撃範囲を走査し、一つでも範囲内にあれば成功を返す
		for (int i = 0; i < enemy.attackDatas.size(); i++)
		{
			const AIBlackboard::BBAttackData& attack = enemy.attackDatas.at(i);

			if (!IsTargetInAttackRange(blackBord.playerPos, enemy.position, enemy.rotation, attack))
			{
				continue;
			}

			// 敵が攻撃範囲内にいるため、関数を終了し成功を返す
			attackIndex = i;
			isInAttackRange = true;
			break;
		}

		if (isInAttackRange)
		{
			result.MoveDir = float2(0.0f, 0.0f);
			result.IsMove = false;
			result.AttackIndex = attackIndex;
			result.UseAttack = true;
			return NODE_SUCCES;
		}
		else
		{
			result.MoveDir = float2(0.0f, 0.0f);
			result.IsMove = false;
			result.AttackIndex = 0;
			result.UseAttack = false;
			return NODE_FAILURE;
		}

}
