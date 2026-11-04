#include "AISystem.h"
#include "AIData.h"

#include "Components.h"
#include "SystemAssist.h"

using namespace Component;

// Chunk情報をAIManagerに登録するSystem
void AISenseSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager)
{
	(void)a_context;

	AISenseFrame senseFrame;

	Entity player = GetPlayer(a_chunk, float3());

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
		// 今フレームの状態
		AISenseEnemyFrame enemyFrame;

		// トランスフォームを代入
		ComponentHandle<Position> enemyPos = a_chunk.GetComponent<Position>(it);
		enemyFrame.position = float2(enemyPos.Look().x, enemyPos.Look().z);
		ComponentHandle<Rotation> enemyRot = a_chunk.GetComponent<Rotation>(it);
		enemyFrame.rotation = enemyRot.Look().yaw;

		// 行動可能かどうかを代入
		ComponentHandle<ActionMask> enemyMask = a_chunk.GetComponent<ActionMask>(it);
		if (enemyMask.IsValid())
		{
			enemyFrame.canAttack = (enemyMask.Look().allowed & ActionFlag_Attack) != 0;
			enemyFrame.canMove = (enemyMask.Look().allowed & ActionFlag_Move) != 0;
		}
		else
		{
			enemyFrame.canAttack = false;
			enemyFrame.canMove = false;
		}

		senseFrame.enemies[it] = enemyFrame;
	}

	// AI側に書き込む
	a_aiManager.WriteSenseFrame(senseFrame);
}
