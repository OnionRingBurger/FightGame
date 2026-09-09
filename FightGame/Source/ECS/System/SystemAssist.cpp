#include <fstream>

#include "SystemAssist.h"
#include "Components.h"
#include "GameData.h"
#include "Defines.h"
#include "Geometory.h"
#include "json.hpp"

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

Entity GetCameraRig(Chunk& a_chunk)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<CameraRigTag>>();

	for (auto it : view)
	{
		return it;
	}

	return kInvalidEntity;
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

Entity GetLookOnMarker(Chunk& a_chunk)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<LookOnMarkerTag>>();
	for (auto it : view)
	{
		return it;
	}

	return kInvalidEntity;
}

Entity GetCamera(Chunk& a_chunk)
{
	// 事前に登録用カメラを取得
	Entity cameraEntity = kInvalidEntity;
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

void ClearCharacterAttackOnDeath(Chunk& a_chunk, Entity a_entity)
{
	ComponentHandle<AttackStartupAction> startup = a_chunk.GetComponent<AttackStartupAction>(a_entity);
	if (startup.IsValid())
	{
		const Entity telegraphEntity = startup.Look().telegraphEntity;
		if (telegraphEntity != kInvalidEntity
			&& a_chunk.GetComponent<AttackTelegraph>(telegraphEntity).IsValid())
		{
			a_chunk.DeleteChunkEntity(telegraphEntity);
		}
		a_chunk.DeleteChunkComponent(a_entity, AttackStartupAction::kTypeId);
	}

	ComponentHandle<AttackAction> attackAction = a_chunk.GetComponent<AttackAction>(a_entity);
	if (attackAction.IsValid())
	{
		const Entity attackEntity = attackAction.Look().attackEntity;
		if (attackEntity != kInvalidEntity
			&& (a_chunk.GetComponent<PlayerAttackTag>(attackEntity).IsValid()
				|| a_chunk.GetComponent<EnemyAttackTag>(attackEntity).IsValid()))
		{
			a_chunk.DeleteChunkEntity(attackEntity);
		}
		a_chunk.DeleteChunkComponent(a_entity, AttackAction::kTypeId);
	}

	if (a_chunk.GetComponent<AttackWaitAction>(a_entity).IsValid())
	{
		a_chunk.DeleteChunkComponent(a_entity, AttackWaitAction::kTypeId);
	}

	ComponentView view = a_chunk.GetView<ComponentTypes<AttackInstance>>();
	for (auto it : view)
	{
		const ComponentHandle<AttackInstance> attackInstance = a_chunk.GetComponent<AttackInstance>(it);
		if (attackInstance.Look().owner != a_entity) continue;

		a_chunk.DeleteChunkEntity(it);
	}
}


void RegisterAllAttackSectorsFromAttackData(int a_circumferenceCount)
{
	std::ifstream stream(kAttackDataPath);
	if (!stream.is_open()) return;

	nlohmann::json json;
	stream >> json;

	for (auto it = json.begin(); it != json.end(); ++it)
	{
		const std::string key = it.key();
		const nlohmann::json& entry = it.value();
		if (!entry.is_object()) continue;

		Geometory::RegisterSector(
			key,
			entry.value("MinLength", 0.0f),
			entry.value("MaxLength", 0.0f),
			entry.value("Angle", 0.0f),
			entry.value("MaxHeight", 0.0f),
			entry.value("MaxLowness", 0.0f),
			a_circumferenceCount);
	}
}

void AddShakeEffect(Chunk& a_chunk, Entity a_entity, float3 a_power, float3 a_amp, float a_time, float a_rate)
{
	ComponentHandle<ShakeComponent> hitterShake = a_chunk.GetComponent<ShakeComponent>(a_entity);
	if (hitterShake.IsValid())
	{
		hitterShake->shakePower = a_power;
		hitterShake->shakeAmplitude = a_amp;
		hitterShake->elapsedTime = 0.0f;
		hitterShake->shakeTime = a_time;
		hitterShake->decayRate = a_rate;
	}
	else
	{
		a_chunk.AddComponent(a_entity, ShakeComponent(a_power, a_amp, a_time, a_rate));
	}
}

void SafeDeleteEffectEntity(Chunk& a_chunk, Entity a_effect)
{
	// TODO 後でやる
}


void NewSceneSpawn(ComponentsSerialize& a_serialize, Chunk& a_chunk, AIManager& aiManager, std::string a_newSceneName, float3 a_offset)
{
	std::vector<Entity> newEntity = LoadJsonComponent(a_chunk, a_serialize, a_newSceneName);
	for (auto it : newEntity)
	{
		ComponentHandle<LeapPosComponent> leap = a_chunk.GetComponent<LeapPosComponent>(it);
		if (leap.IsValid())
		{
			leap->leapStartPos += a_offset;
			leap->leapEndPos += a_offset;
		}

		ComponentHandle<MotionTransform> motion = a_chunk.GetComponent<MotionTransform>(it);
		ComponentHandle<PosePosState> posState = a_chunk.GetComponent<PosePosState>(it);
		if (!motion.IsValid() || posState.IsValid()) continue;
		motion->motionPos += a_offset;

		ComponentHandle<EnemyTag> enemyTag = a_chunk.GetComponent<EnemyTag>(it);
		if (!enemyTag.IsValid()) continue;


		ComponentHandle<AttackKeyLoad> keyLoad = a_chunk.GetComponent<AttackKeyLoad>(it);
		if (keyLoad.IsValid())
		{
			std::ifstream stream(kAttackDataPath);
			if (!stream.is_open()) continue;
			nlohmann::json json;
			stream >> json;
			AttackStatus status;
			for (auto attackIt : keyLoad.Look().attackKeys)
			{
				status.attackPowers.push_back(
					AttackPower(
						json[attackIt].value("MinLength", 0.0f),
						json[attackIt].value("MaxLength", 0.0f),
						json[attackIt].value("Angle", 0.0f),
						json[attackIt].value("MaxHeight", 0.0f),
						json[attackIt].value("MaxLowness", 0.0f),
						json[attackIt].value("DamageValue", 0.0f),
						json[attackIt].value("MotionTime", 0.0f),
						json[attackIt].value("LifeTime", 0.0f),
						float3(
							json[attackIt].value("FollowOffsetX", 0.0f),
							json[attackIt].value("FollowOffsetY", 0.0f),
							json[attackIt].value("FollowOffsetZ", 0.0f)
						),
						json[attackIt].value("WaitTime", 0.0f),
						json[attackIt].value("StartupTime", 0.0f),
						attackIt,
						json[attackIt].value("EndWithOwnerAction", true))

				);

			}

			a_chunk.AddComponent(it, status);
			a_chunk.DeleteChunkComponent(it, AttackKeyLoad::kTypeId);
			aiManager.RegisterAI(it, "Enemy");
		
		}
	}
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

void ResetFixedRotState(ComponentHandle<FixedResult> fixedResult, ComponentHandle<MotionResult> motionResult)
{
	motionResult->rotOffset += fixedResult.Look().newRot;
	fixedResult->newRot = float3(0.0f, 0.0f, 0.0f);
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

