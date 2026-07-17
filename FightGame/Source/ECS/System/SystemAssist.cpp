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

float3 GetEntityWorldPos(Chunk& a_chunk, Entity a_entity)
{
	ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(a_entity);
	if (pos.IsValid()) return TOFLOAT3(pos.Look());

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

