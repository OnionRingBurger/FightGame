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

// !!!New!!!
void MoveInputResolveSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager)
{
	// 入力を取得
	const Axis& leftAxis = a_context.input.GetLeftAxis().magnitube >= a_context.input.GetKeyAxis().magnitube
		? a_context.input.GetLeftAxis()
		: a_context.input.GetKeyAxis();
	// !!!New!!!
	const bool rightAttackTrigger = a_context.input.IsRegisterTrigger("RightAttack");
	const bool leftAttackTrigger = a_context.input.IsRegisterTrigger("LeftAttack");
	const bool jumpTrigger = a_context.input.IsRegisterTrigger("Jump");
	const bool guardPress = a_context.input.IsRegisterPress("Guard");

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
		result->attackIndex = 0;

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
			// 右攻撃=0、左攻撃=1。同時押しは右優先
			if (rightAttackTrigger)
			{
				result->useAttack = true;
				result->attackIndex = 0;
			}
			else if (leftAttackTrigger)
			{
				result->useAttack = true;
				result->attackIndex = 1;
			}
			break;
		case InputOrigin::AI:
		{
			const AIResult aiResult = a_aiManager.ReadResult(it);
			result->useAttack = aiResult.UseAttack;
			result->attackIndex = aiResult.AttackIndex;
			break;
		}
		case InputOrigin::None:
		default:
			break;
		}

		switch (source.Look().jump)
		{
			case InputOrigin::Device:
			result->useJump = jumpTrigger;
			
			break;

			case InputOrigin::AI:
			result->useJump = false;
			break;

		default:
			break;
		}

		switch (source.Look().guard)
		{
			case InputOrigin::Device:
			result->useGuard = guardPress;
			
			break;

			case InputOrigin::AI:
			result->useGuard = false;
			break;

		default:
			break;
		}
	}
}
