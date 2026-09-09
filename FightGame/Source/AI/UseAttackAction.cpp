#include "AI/UseAttackAction.h"

// !!!New!!!
UseAttackAction::UseAttackAction()
{
}

UseAttackAction::~UseAttackAction()
{
}

Node::Status UseAttackAction::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{

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

		// 事前に要素を計算する
		const float2 toPlayer(
			blackBord.playerPos.x - enemy.position.x,
			blackBord.playerPos.y - enemy.position.y);
		const float dist = GetLength(toPlayer);
		float2 nomalizeVec = Normalize(toPlayer);
		float2 enemyFrontVec = AngleToVector(-enemy.rotation + 90.0f);
		float dot = nomalizeVec.x * enemyFrontVec.x + nomalizeVec.y * enemyFrontVec.y;
		float toPlayerAngle = acosf(dot) * DEG;
		
		bool isInAttackRange = false;

		// 攻撃範囲を走査し、一つでも範囲内にあれば成功を返す
		for (int i = 0; i < enemy.attackDatas.size(); i++)
		{
			const AIBlackboard::BBAttackData& attack = enemy.attackDatas.at(i);
			// 射程距離外だった場合抜ける
			if (!(dist >= attack.minLength) || !(dist <= attack.maxLength))
			{
				continue;
			}
	
			// 攻撃角より外にいた場合場合抜ける
			float halfAttackAngle = attack.angle * 0.5f;
			if (halfAttackAngle < toPlayerAngle)
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
