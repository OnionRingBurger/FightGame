#include "EnemySystem.h"
#include "MathAssist.h"
#include "Components.h"
#include "Sound.h"
#include "SystemAssist.h"
#include "CharacterSystem.h"
#include "GameData.h"

#include <cmath>

using namespace Component;

float3 GetPlayerViewDirection(Chunk& a_chunk, Entity a_player);
float2 GetScreenDirectionFromView(const float3& a_viewDir, const float3& a_toTarget);



PlayerHitViewInfo EvaluatePlayerHitView(Chunk& a_chunk, Entity a_player, Entity a_hitEntity, float a_minAngleDeg)
{
	PlayerHitViewInfo result;

	float3 playerPos = GetEntityWorldPos(a_chunk, a_player);
	float3 targetPos = GetEntityWorldPos(a_chunk, a_hitEntity);
	float3 toTarget = targetPos - playerPos;
	float toTargetLen = NormalizeLength(toTarget.x, toTarget.y, toTarget.z);
	if (toTargetLen <= 0.001f) return result;

	// 単位ベクトルにする
	toTarget.x /= toTargetLen;
	toTarget.y /= toTargetLen;
	toTarget.z /= toTargetLen;
	// 視点方向を取得
	float3 viewDir = GetPlayerViewDirection(a_chunk, a_player);
	float dot = DotFloat3(viewDir, toTarget);
	// 角度を比較
	result.isOffView = dot < cosf(a_minAngleDeg * RAD);
	// 自分の画面から見た敵の方向を取得
	result.screenDirection = GetScreenDirectionFromView(viewDir, toTarget);
	return result;
}

bool IsPlayerHitFromOffView(Chunk& a_chunk, Entity a_player, Entity a_hitEntity, float a_minAngleDeg)
{
	return EvaluatePlayerHitView(a_chunk, a_player, a_hitEntity, a_minAngleDeg).isOffView;
}



void EnemyDeadSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_response)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<EnemyTag, DeadState>, ComponentTypes<BossTag>>();
	Entity camera = GetCamera(a_chunk);
	for (auto it : view)
	{
		ComponentHandle<DeadState> dead = a_chunk.GetComponent<DeadState>(it);
		if (!dead.Look().isDead) continue;

		a_chunk.DeleteChunkComponent(it, DeadState::kTypeId);

		
		// 画面エフェクトを出す
		a_response.AddStopTime(6.0f, 6.0f, 0.03f);
		float3 shakePower = float3(0.12f, 0.12f, 0.00f);
		float3 shakeAmp = float3(0.20f, 0.20f, 0.0f);
		float shakeTime = 17.0f;
		ComponentHandle<ShakeComponent> cameraShake = a_chunk.GetComponent<ShakeComponent>(camera);
		if (cameraShake.IsValid())
		{
			cameraShake->shakePower = shakePower;
			cameraShake->shakeAmplitude = shakeAmp;
			cameraShake->elapsedTime = 0.0f;
			cameraShake->shakeTime = shakeTime;
		}
		else
		{
			a_chunk.AddComponent(camera, ShakeComponent(shakePower, shakeAmp, shakeTime, 0.5f));
		}

		Entity sound = a_chunk.CreateNewEntity(
			SoundKey(false, 6.0f, "enemydead")
		);

		ComponentHandle<LifeTime> lifeTime = a_chunk.GetComponent<LifeTime>(it);
		if (lifeTime.IsValid())
		{
			lifeTime->time = 6.0f;
		}
		else
		{
			a_chunk.AddComponent(it, LifeTime(6.0f));
		}
		

		Entity finalEffectTime = a_chunk.CreateNewEntity(
			ClearTarget(),
			LifeTime(50.0f)
		);

		a_chunk.AddComponent(it, EfkEffectArea(std::vector<std::string>{kHitEffect}, std::vector<std::string>{"finalalive"}, 0.2f, 2.5f, 1.0f));
		a_chunk.AddComponent(it, LifeTimeEndEffect(false, "", true, kDeadBrokenEffectKey));
	}

}

void BossDeadSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_response)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<EnemyTag, BossTag, DeadState>>();
	Entity camera = GetCamera(a_chunk);
	for (auto it : view)
	{
		ComponentHandle<DeadState> dead = a_chunk.GetComponent<DeadState>(it);
		if (!dead.Look().isDead) continue;

		a_chunk.DeleteChunkComponent(it, DeadState::kTypeId);


		// 画面エフェクトを出す
		a_response.AddStopTime(6.0f, 6.0f, 0.03f);
		float3 shakePower = float3(0.22f, 0.22f, 0.00f);
		float3 shakeAmp = float3(0.20f, 0.20f, 0.0f);
		float shakeTime = 30.0f;
		
		AddShakeEffect(a_chunk, camera, shakePower, shakeAmp, shakeTime, 0.5f);

		// ResetSound();

		Entity sound = a_chunk.CreateNewEntity(
			SoundKey(false, 300.0f * 0.03f, "bossdead")
		);

		Entity sound2 = a_chunk.CreateNewEntity(
			SoundKey(false, 300.0f * 0.03f, "bossdead2")
		);

		Entity sound3 = a_chunk.CreateNewEntity(
			SoundKey(false, 300.0f * 0.03f, "dead2")
		);

		// 振動、一時停止を追加
		a_response.AddStopTime(300.0f, 300.0f, 0.03f);
		AddShakeEffect(a_chunk, it, float3(0.02f, 0.02f, 0.02f), float3(2.5f, 2.5f, 2.5f), 600.0f, 15.5f);


		Entity effect = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
			EfkEffectKey("bossdead", false, 300.0f * 0.03f),
			PosePosState(POSE_POS_FOLLOW),
			FollowPosition(0.0f, 0.0f, 0.0f, it)
		);

		// 生存時間を決定
		ComponentHandle<LifeTime> lifeTime = a_chunk.GetComponent<LifeTime>(it);
		if (lifeTime.IsValid())
		{
			lifeTime->time = 300.0f * 0.03f;
		}
		else
		{
			a_chunk.AddComponent(it, LifeTime(300.0f * 0.03f));
		}

		Entity finalEffectTime = a_chunk.CreateNewEntity(
			ClearTarget(),
			LifeTime(360.0f * 0.03f + 60.0f)
		);

		// エフェクトを生成
		a_chunk.AddComponent(it, EfkEffectArea({ kHitEffect }, std::vector<std::string>{"finalalive", "finalalive2", "finalalive3"}, 0.5f, 15.0f * 0.03f, 1.05f));
		a_chunk.AddComponent(it, LifeTimeEndEffect(false, "", true, kBossDeadBrokenEffectKey));

	}
}


void EnemyBulletDeadSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<EnemyBulletTag, DeadState>>();
	for (auto it : view)
	{
		ComponentHandle<DeadState> dead = a_chunk.GetComponent<DeadState>(it);
		if (!dead.Look().isDead) continue;

		ComponentHandle<MotionTransform> enemyBulletMotion = a_chunk.GetComponent<MotionTransform>(it);
		float3 areaPos;
		if (enemyBulletMotion.IsValid()) areaPos = enemyBulletMotion.Look().motionPos;

		Entity rail = GetRail(a_chunk);
		Entity area = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(areaPos.x, areaPos.y, areaPos.z), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
			GhostAreaComponent(10.0f),
			LifeTime(50.0f),
			PosePosState(POSE_POS_RAIL),
			RailUser(rail),
			DeadState()
		);

		a_chunk.DeleteChunkEntity(it);
	}
}



// Playerの向いている向きを取得する
float3 GetPlayerViewDirection(Chunk& a_chunk, Entity a_player)
{
	ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(a_player);
	if (rotation.IsValid()) return GetForward(TOFLOAT3(rotation.Look()));

	return float3(0.0f, 0.0f, 1.0f);
}

// 自身の向きからターゲットの方向を取得
float2 GetScreenDirectionFromView(const float3& a_viewDir, const float3& a_toTarget)
{
	const float3 worldUp(0.0f, 1.0f, 0.0f);
	// 受け取った単位ベクトルと見ている方向から横方向を取得
	float3 viewRight = Cross(worldUp, a_viewDir);
	float viewRightLen = NormalizeLength(viewRight.x, viewRight.y, viewRight.z);
	// 真上、真下を向いている場合値が取れていないため別軸で判断
	if (viewRightLen <= 0.001f)
	{
		viewRight = Cross(a_viewDir, float3(0.0f, 0.0f, 1.0f));
		viewRightLen = NormalizeLength(viewRight.x, viewRight.y, viewRight.z);
	}
	if (viewRightLen <= 0.001f) return float2(0.0f, -1.0f);

	// 単位ベクトルを取得
	viewRight.x /= viewRightLen;
	viewRight.y /= viewRightLen;
	viewRight.z /= viewRightLen;
	// 今度は右方向から画面の上方向を取得
	float3 viewUp = Normalize(Cross(a_viewDir, viewRight));

	// 敵の位置から内積を取得(自分の右、上方向と敵への方向ベクトルが等しいほど1に近づき距離が取れる)
	float screenX = DotFloat3(a_toTarget, viewRight);
	float screenY = DotFloat3(a_toTarget, viewUp);
	// 距離を取得
	float screenLen = sqrtf(screenX * screenX + screenY * screenY);
	if (screenLen <= 0.001f)
	{
		return DotFloat3(a_viewDir, a_toTarget) < 0.0f ? float2(0.0f, -1.0f) : float2(0.0f, 1.0f);
	}
	// 距離を使って正規化
	return float2(screenX / screenLen, screenY / screenLen);
}






