#include "AISystem.h"
#include "AIData.h"

#include "Components.h"
#include "SystemAssist.h"

using namespace Component;

// Chunk情報をAIManagerに登録するSystem
void AISenseSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager)
{

	AISenseFrame senseFrame;

	Entity player =	GetPlayer(a_chunk, float3());

	// プレイヤーのステータスを代入
	ComponentHandle<DeadState> deadState = a_chunk.GetComponent<DeadState>(player);
	senseFrame.isPlayerDead = !deadState.IsValid() || deadState.Look().isDead;

	ComponentHandle<Position> playerPos = a_chunk.GetComponent<Position>(player);
	if (playerPos.IsValid())
	{
		senseFrame.playerPos = float2(playerPos.Look().x, playerPos.Look().z);
	}

	// 敵のステータスを全て代入する
	ComponentView enemyView = a_chunk.GetView<ComponentTypes<EnemyTag, Position, Rotation, AttackStatus, DeadState, AIRole>>();

	for (auto it : enemyView)
	{
		// 敵データを作成し代入
		AIBlackboard::BBEnemyData enemyData;

		// トランスフォーム
		ComponentHandle<Position> enemyPos = a_chunk.GetComponent<Position>(it);
		enemyData.position = float2(enemyPos.Look().x, enemyPos.Look().z);
		ComponentHandle<Rotation> enemyRot = a_chunk.GetComponent<Rotation>(it);
		enemyData.rotation = enemyRot.Look().yaw;

		// 行動可能かどうか
		ComponentHandle<ActionMask> enemyMask = a_chunk.GetComponent<ActionMask>(it);
		if (enemyMask.IsValid())
		{
			enemyData.canAttack = (enemyMask.Look().allowed & ActionFlag_Attack) != 0;
			enemyData.canMove = (enemyMask.Look().allowed & ActionFlag_Move) != 0;
		}
		else
		{
			enemyData.canAttack = false;
			enemyData.canMove = false;
		}

		// 攻撃データを全てコピー
		ComponentHandle<AttackStatus> enemyAttackStatus = a_chunk.GetComponent<AttackStatus>(it);
		ComponentHandle<AIRole> enemyRole = a_chunk.GetComponent<AIRole>(it);

		enemyData.attackDatas.clear();
		for (const AttackPower& power : enemyAttackStatus.Look().attackPowers)
		{
			AIBlackboard::BBAttackData enemyAttackData;
			enemyAttackData.maxLength = power.maxLength;
			enemyAttackData.minLength = power.minLength;
			enemyAttackData.angle = power.angle;
			enemyAttackData.waitTime = power.waitTime;
			enemyAttackData.damageValue = power.damageValue;
			enemyData.attackDatas.push_back(enemyAttackData);
		}
		// ロール登録
		enemyData.distCoefficient = enemyRole.Look().distCoefficient;
		enemyData.farnessCoefficient = enemyRole.Look().farnessCoefficient;
		enemyData.hitCoefficient = enemyRole.Look().hitCoefficient;
		enemyData.stanceCoefficient = enemyRole.Look().stanceCoefficient;

		// 対応する位置に上書き
		senseFrame.enemyDatas[it] = enemyData;
	}

	// AI側に書き込む
	a_aiManager.WriteSense(senseFrame);
	
}
