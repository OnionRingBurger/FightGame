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
	ComponentView enemyView = a_chunk.GetView<ComponentTypes<EnemyTag, Position, Rotation, AttackPower, DeadState>>();

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

		// 攻撃データも作成し代入する
		AIBlackboard::BBAttackData enemyAttackData;
		ComponentHandle<AttackPower> enemyAttackPower = a_chunk.GetComponent<AttackPower>(it);
		// 攻撃範囲
		enemyAttackData.maxLength = enemyAttackPower.Look().maxLength;
		enemyAttackData.minLength = enemyAttackPower.Look().minLength;
		enemyAttackData.angle = enemyAttackPower.Look().angle;
		// 火力、継続時間
		enemyAttackData.waitTime = enemyAttackPower.Look().waitTime;
		enemyAttackData.damageValue = enemyAttackPower.Look().damageValue;
		// 敵データに代入
		enemyData.attackData = enemyAttackData;

		// 対応する位置に上書き
		senseFrame.enemyDatas[it] = enemyData;
	}

	// AI側に書き込む
	a_aiManager.WriteSense(senseFrame);
	
}

// !!!New!!!
void MoveInputResolveSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager)
{
	// 入力を取得
	const Axis& leftAxis = a_context.input.GetLeftAxis().magnitube >= a_context.input.GetKeyAxis().magnitube
		? a_context.input.GetLeftAxis()
		: a_context.input.GetKeyAxis();
	const bool shotTrigger = a_context.input.IsRegisterTrigger("Shot");
	Entity cameraEntity = GetCamera(a_chunk);

	ComponentView view = a_chunk.GetView<ComponentTypes<MoveInputResult, InputSource>>();

	for (auto it : view)
	{
		ComponentHandle<MoveInputResult> result = a_chunk.GetComponent<MoveInputResult>(it);
		const ComponentHandle<InputSource> source = a_chunk.GetComponent<InputSource>(it);

		result->moveDir = float2(0.0f, 0.0f);
		result->magnitube = 0.0f;
		result->isInput = false;
		result->useAttack = false;

		// !!!New!!!
		switch (source.Look().move)
		{
		case InputOrigin::Device:
		{
			if (leftAxis.magnitube == 0.0f)
			{
				break;
			}

			DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
			// カメラの角度を取得
			ComponentHandle<Rotation> cameraRot = a_chunk.GetComponent<Rotation>(cameraEntity);
			if (cameraRot.IsValid())
			{
				matrix *= DirectX::XMMatrixRotationRollPitchYaw(0.0f, cameraRot.Look().yaw * RAD, 0.0f);
			}
			// カメラの回転から見た前方方向のベクトルを取得する
			DirectX::XMVECTOR forwardVector = DirectX::XMVector3TransformNormal({ 0, 0, 1 }, matrix);
			// カメラから見た前方方向にたいして入力をかける
			forwardVector = DirectX::XMVectorScale(forwardVector, leftAxis.y);
			// 横方向も同様の計算を行う
			DirectX::XMVECTOR besideVector = DirectX::XMVector3TransformNormal({ 1, 0, 0 }, matrix);
			besideVector = DirectX::XMVectorScale(besideVector, leftAxis.x);

			// ベクトルを加算して方向を取得
			DirectX::XMVECTOR moveVector = DirectX::XMVectorAdd(forwardVector, besideVector);
			DirectX::XMFLOAT3 moveFloat3;
			XMStoreFloat3(&moveFloat3, moveVector);

			// 数値を代入
			result->moveDir = { moveFloat3.x, moveFloat3.z };
			result->magnitube = leftAxis.magnitube;
			result->isInput = true;
			break;
		}
		case InputOrigin::AI:
		{
			const AIResult aiResult = a_aiManager.ReadResult(it);
			result->moveDir = aiResult.MoveDir;
			result->magnitube = 1.0f;
			result->isInput = aiResult.IsMove;
			break;
		}
		case InputOrigin::None:
		default:
			break;
		}

		// !!!New!!!
		switch (source.Look().attack)
		{
		case InputOrigin::Device:
			result->useAttack = shotTrigger;
			break;
		case InputOrigin::AI:
			result->useAttack = a_aiManager.ReadResult(it).UseAttack;
			break;
		case InputOrigin::None:
		default:
			break;
		}
	}
}
