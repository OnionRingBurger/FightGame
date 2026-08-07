#include "SystemAssist.h"
#include "Components.h"

using namespace Component;

Entity GetPlayer(Chunk& a_chunk, const float3 a_position)
{
	ComponentView view = a_chunk.GetView <ComponentTypes<PlayerTag, Position>>();
	Entity player = kInvalidEntity;
	float dir = FLT_MAX;
	for (auto it : view)
	{
		ComponentHandle<Position> itPosition = a_chunk.GetComponent<Position>(it);
		float3 relativeDistance(
			itPosition.Look().x - a_position.x,
			itPosition.Look().y - a_position.y,
			itPosition.Look().z - a_position.z
		);
		float itDir = NormalizeLength(
			relativeDistance.x,
			relativeDistance.y,
			relativeDistance.z
		);

		if (itDir < dir)
		{
			player = it;
		}
	}

	return player;
}

Entity GetRail(Chunk& a_chunk)
{
	ComponentView view = a_chunk.GetView <ComponentTypes<RailComponent>>();
	Entity a_rail = kInvalidEntity;

	for (auto it : view)
	{
		a_rail = it;
		break;
	}

	return a_rail;
}

Entity GetCamera(Chunk& a_chunk)
{
	// 事前に登録用カメラを取得
	Entity cameraEntity;
	ComponentHandle<Camera> cameraComponent;

	ComponentView view = a_chunk.GetView<ComponentTypes<Camera>>();
	// 探索
	for (auto it : view)
	{
		ComponentHandle<Camera> itCamera = a_chunk.GetComponent<Camera>(it);

		if (!cameraComponent.IsValid() || (itCamera.Look().cameraPriority > cameraComponent.Look().cameraPriority))
		{
			cameraEntity = it;
			cameraComponent = itCamera;
		}
	}
	// カメラがなかった場合空のエンティティを返す
	if (!cameraComponent.IsValid()) return kInvalidEntity;

	return cameraEntity;
}

bool IsActionAllowed(Chunk& a_chunk, Entity a_entity, ActionFlag a_flag)
{
	const ComponentHandle<ActionMask> mask = a_chunk.GetComponent<ActionMask>(a_entity);
	if (!mask.IsValid()) return true;
	return (mask.Look().allowed & a_flag) != 0;
}

// 攻撃解除可能だった場合キャンセルする
void CancelPlayerAttackIfAble(Chunk& a_chunk, Entity a_entity)
{
	// 前隙中はキャンセル不可
	if (a_chunk.GetComponent<AttackStartupAction>(a_entity).IsValid()) return;

	ComponentHandle<AttackAction> attackAction = a_chunk.GetComponent<AttackAction>(a_entity);
	if (!attackAction.IsValid()) return;

	// AttackWaitAction 中はキャンセル不可
	if (a_chunk.GetComponent<AttackWaitAction>(a_entity).IsValid()) return;

	Entity attackEntity = attackAction.Look().attackEntity;
	// 攻撃判定が残っていた場合削除する
	if (a_chunk.GetComponent<PlayerAttackTag>(attackEntity).IsValid()
		|| a_chunk.GetComponent<EnemyAttackTag>(attackEntity).IsValid())
	{
		a_chunk.DeleteChunkEntity(attackEntity);
	}

	a_chunk.DeleteChunkComponent(a_entity, AttackAction::kTypeId);
	a_chunk.DeleteChunkComponent(a_entity, AttackWaitAction::kTypeId);
}

float3 GetEntityWorldPos(Chunk& a_chunk, Entity a_entity)
{
	ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(a_entity);
	if (pos.IsValid()) return TOFLOAT3(pos.Look());

	return float3();
}

float3 GetEntityPosePos(Chunk& a_chunk, Entity a_entity)
{
	const ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(a_entity);
	if (pose.IsValid()) return pose.Look().pos;

	return GetEntityWorldPos(a_chunk, a_entity);
}

float3 GetEntityPoseRot(Chunk& a_chunk, Entity a_entity)
{
	const ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(a_entity);
	if (pose.IsValid()) return pose.Look().rot;

	const ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(a_entity);
	if (rotation.IsValid()) return TOFLOAT3(rotation.Look());

	return float3();
}

void ResetFixedPosState(ComponentHandle<FixedResult> fixedResult, ComponentHandle<MotionResult> motionResult)
{
	motionResult->posOffset += fixedResult.Look().newPos;
	fixedResult->newPos = float3(0.0f, 0.0f, 0.0f);
}

float3 GetLocalOffset(float3 worldOffset, float3 rotation)
{
	// 行列を使って注視点を求める
	float pitch = rotation.x * RAD;
	float yaw = rotation.y * RAD;
	float roll = rotation.z * RAD;

	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	float3 localOffset = worldOffset;

	DirectX::XMVECTOR local =
		DirectX::XMVectorSet(localOffset.x, localOffset.y, localOffset.z, 0.0f);

	DirectX::XMVECTOR world =
		DirectX::XMVector3TransformNormal(local, rot);

	DirectX::XMFLOAT3 dxWorldOffset;
	DirectX::XMStoreFloat3(&dxWorldOffset, world);

	return float3(dxWorldOffset.x, dxWorldOffset.y, dxWorldOffset.z);
}

