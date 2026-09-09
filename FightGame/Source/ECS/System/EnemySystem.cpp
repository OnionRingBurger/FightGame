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

void SpawnEnemy(float3 a_targetPos, SpawnType a_type, Chunk& a_chunk);

void SpawnNormalEnemy(Chunk& a_chunk, float3 a_targetPos, float3 a_spawnRailPosNormalized);
void SpawnShooterEnemy(Chunk& a_chunk, float3 a_targetPos);
void SpawnGroupEnemy(Chunk& a_chunk, float3 a_targetPos, float3 a_spawnRailPosNormalized, int a_groupEnemyCount);

float3 RandomEnemyPick(int randomWidth, int randomHeight, int randomDepth);

bool SpawnEnemyBullet(Chunk& a_chunk, float3 a_shooterPos, Entity a_player);


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

void EnemyAttackSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_systemResponse)
{
	Entity camera = GetCamera(a_chunk);

	ComponentView view = a_chunk.GetView<ComponentTypes<PlayerTag, HitInfomation, HitPoint>>();
	for (auto it : view)
	{
		ComponentHandle<HitInfomation> info = a_chunk.GetComponent<HitInfomation>(it);

		for (const auto& hitIt : info.Look().hitResults)
		{
			Entity hitEntity = hitIt.hitEntity;
			ComponentHandle<AddDamageComponent> damage = a_chunk.GetComponent<AddDamageComponent>(hitEntity);

			ComponentHandle<EnemyTag> enemyTag = a_chunk.GetComponent<EnemyTag>(hitEntity);
			ComponentHandle<EnemyBulletTag> enemyBulletTag = a_chunk.GetComponent<EnemyBulletTag>(hitIt.hitEntity);

			if (!damage.IsValid()) continue;
			PlaySound(LoadSound("Assets/Sound/damage.mp3"));
			PlaySound(LoadSound("Assets/Sound/damage2.mp3"));
			ComponentHandle<HitPoint>hitPoint = a_chunk.GetComponent<HitPoint>(it);
			hitPoint->currentHP -= damage.Look().damageValue;
			ComponentView view = a_chunk.GetView<ComponentTypes<CameraTag, ShakeComponent>>();
			for (auto cameraIt : view)
			{
				ComponentHandle<ShakeComponent> shake = a_chunk.GetComponent<ShakeComponent>(cameraIt);
				shake->shakeTime = 10.0f;
			}

			a_systemResponse.AddStopTime(20.0f, 20.0f, 0.01f);

			

			if (!enemyTag.IsValid() && !enemyBulletTag.IsValid()) continue;

			Entity createDamageEffect = a_chunk.CreateNewEntity(
				CreateEffect(DAMAGE_EFFECT)
			);


			ComponentHandle<DeadState> enemyDead = a_chunk.GetComponent<DeadState>(hitIt.hitEntity);
			if (enemyDead.IsValid())
			{
				CharacterKill(a_chunk, hitIt.hitEntity);
			}

			PlayerHitViewInfo hitView = EvaluatePlayerHitView(a_chunk, it, hitEntity, kDamageDirectionMinAngleDeg);
			if (hitView.isOffView)
			{
				// ベクトルからアングルを取得
				float targetAngle = (-atan2f(hitView.screenDirection.y, hitView.screenDirection.x) + PI / 2) * DEG;
				// エフェクトを作成
				Entity createDamageDirEffect = a_chunk.CreateNewEntity(
					CreateEffect(DAMAGE_DIRECTION, float2(), targetAngle)
				);

			}
		}

		for (auto triggerIt : info.Look().triggerResults)
		{
			ComponentHandle<DarkTag> darkTag = a_chunk.GetComponent<DarkTag>(triggerIt.triggerEntity);
			if (darkTag.IsValid())
			{

				a_chunk.AddComponent(it, FollowRotation(0.0f, 180.0f, 0.0f, triggerIt.triggerEntity));

				Entity gameOverUI = a_chunk.CreateNewEntity();
								// チャンクを変更を出す
				Entity chunkChange = a_chunk.CreateNewEntity(
					ChunkChange(true, "Result"),
					DelayChunkChange(240.0f)
				);
				// ゲームオーバーエフェクトを出す
				Entity createDarkEffect = a_chunk.CreateNewEntity(
					CreateEffect(DARKFADE_UP)
				);
				PlaySound(LoadSound("Assets/Sound/dark.mp3"));

			}
		}

	}

}

void EnemyShooterSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView enemyView = a_chunk.GetView<ComponentTypes<EnemyTag, EnemyShooter, MotionTransform, Position>>();
	for (auto it : enemyView)
	{
		ComponentHandle<EnemyShooter> shooter = a_chunk.GetComponent<EnemyShooter>(it);
		shooter->shotTimer = std::max(shooter.Look().shotTimer - a_context.deltaTime, 0.0f);

		if (shooter.Look().shotTimer > 0.0f) continue;
		shooter->shotTimer = shooter.Look().maxShotWaitTime;
		ComponentHandle<MotionTransform> motion = a_chunk.GetComponent<MotionTransform>(it);
		ComponentHandle<Position> position = a_chunk.GetComponent<Position>(it);
		Entity player = GetPlayer(a_chunk, TOFLOAT3(position));

		SpawnEnemyBullet(a_chunk, motion.Look().motionPos, player);
	}
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

void EnemyFearSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView playerView = a_chunk.GetView<ComponentTypes<PlayerTag, Position>>();

	ComponentHandle<Position> playerPos;
	for (auto it : playerView)
	{
		playerPos = a_chunk.GetComponent<Position>(it);
		break;
	}


	ComponentView enemyView = a_chunk.GetView<ComponentTypes<EnemyTag, Position>>();
	for (auto it : enemyView)
	{

	}
}


void EnemyPlayerSearch(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<EnemyTag, Ray, RayInfomation>>();

	for (auto it : view)
	{
		ComponentHandle<RayInfomation> info = a_chunk.GetComponent<RayInfomation>(it);

		ComponentHandle<PlayerTag> playerTag = a_chunk.GetComponent<PlayerTag>(info.Look().otherEntity);
		if (!playerTag.IsValid()) continue;

		a_chunk.AddComponent(it, LookComponent(info.Look().otherEntity));
		a_chunk.AddComponent(it, MoveForward(1.0f));

		a_chunk.DeleteChunkComponent(it, Ray::kTypeId);
		a_chunk.DeleteChunkComponent(it, RayInfomation::kTypeId);

		PlaySound(LoadSound("Assets/Sound/search.mp3"));
		PlaySound(LoadSound("Assets/Sound/search2.mp3"));
	}
}

void EnemySpawnSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<EnemySpawner, Position>>();

	for (auto it : view)
	{
		ComponentHandle<EnemySpawner> spawner = a_chunk.GetComponent<EnemySpawner>(it);

		spawner->spawnTime -= a_context.deltaTime;
		if (spawner.Look().spawnTime > 0.0f)
		{
			continue;
		}

		spawner->spawnTime = spawner.Look().maxSpawnTime;
		BitFlag flag = spawner.Look().flag;
		std::vector<SpawnType> spawn;
		if (flag == SPAWN_ENEMY_DEFAULT)
		{
			for (int i = 0; i < MAX_SPAWN_TYPE; i++)
			{
				spawn.push_back((SpawnType)i);
			}
		}
		else
		{
			if (flag & SPAWN_ENEMY_FORWARD)
			{
				spawn.push_back(SPAWN_NORMAL_FORWARD_1);
				spawn.push_back(SPAWN_NORMAL_FORWARD_2);
				spawn.push_back(SPAWN_NORMAL_FORWARD_3);
			}

			if (flag & SPAWN_ENEMY_SIDE)
			{
				spawn.push_back(SPAWN_NORMAL_LEFT_1);
				spawn.push_back(SPAWN_NORMAL_RIGHT_1);
			}

			if (flag & SPAWN_ENEMY_BODYSLAM)
			{
				spawn.push_back(SPAWN_BODYSLAM_FOWERD_1);
				spawn.push_back(SPAWN_BODYSLAM_FOWERD_2);
			}

			if (flag & SPAWN_ENEMY_STANDBY)
			{
				spawn.push_back(SPAWN_STANDBY_LEFT_1);
				spawn.push_back(SPAWN_STANDBY_RIGHT_1);
				spawn.push_back(SPAWN_STANDBY_UP_1);
			}

			if (flag & SPAWN_ENEMY_SHOOTER)
			{
				spawn.push_back(SPAWN_SHOOTER_1);
			}

			if (flag & SPAWN_ENEMY_GROUP)
			{
				spawn.push_back(SPAWN_GROUP_1);
			}
		}

		if (spawn.size() == 0) continue;

		int random = rand() % spawn.size();

		ComponentHandle<Position> position = a_chunk.GetComponent<Position>(it);
		float3 posFloat(
			position.Look().x,
			position.Look().y,
			position.Look().z
		);


		SpawnEnemy(posFloat, spawn.at(random), a_chunk);
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

void SpawnEnemy(float3 a_targetPos, SpawnType a_type, Chunk& a_chunk)
{
	switch (a_type)
	{
	case SPAWN_NORMAL_FORWARD_1:
	{
		SpawnNormalEnemy(a_chunk, a_targetPos, float3(0.2f, -0.3f, 0.9f));
	}
	break;

	case SPAWN_NORMAL_FORWARD_2:
	{
		SpawnNormalEnemy(a_chunk, a_targetPos, float3(-0.5f, -0.7f, 0.9f));
	}
	break;

	case SPAWN_NORMAL_FORWARD_3:
	{
		SpawnNormalEnemy(a_chunk, a_targetPos, float3(-0.1f, 0.5f, 0.9f));
	}
	break;

	case SPAWN_NORMAL_LEFT_1:
	{
		SpawnNormalEnemy(a_chunk, a_targetPos, float3(-0.9f, -0.3f, 0.7f));
	}
	break;

	case SPAWN_NORMAL_RIGHT_1:
	{
		SpawnNormalEnemy(a_chunk, a_targetPos, float3(0.9f, -0.3f, 0.6f));
	}
	break;

	case SPAWN_BODYSLAM_FOWERD_1:
	{
		Entity enemy = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(a_targetPos.x + 2.0f, a_targetPos.y + 5.5f, a_targetPos.z + 150.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
			BoxCollider(float3{ 0.0f,0.0f,0.0f }, float3{ 4.0f, 7.0f,4.0f }),
			OBBCollider(OBB_PushOutLocked),
			EnemyTag{},
			Velocity(0.0f, 0.0f, -1.0f),
			LifeTime(1200.0f),
			ModelKey("ghost_FINAL")
		);

		PlaySound(LoadSound("Assets/Sound/spawn2.mp3"));
	}
	break;

	case SPAWN_BODYSLAM_FOWERD_2:
	{
		Entity enemy = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(a_targetPos.x - 3.1f, a_targetPos.y + 5.5f, a_targetPos.z + 150.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
			BoxCollider(float3{ 0.0f,0.0f,0.0f }, float3{ 4.0f, 7.0f,4.0f }),
			OBBCollider(OBB_PushOutLocked),
			EnemyTag{},
			Velocity(0.0f, 0.0f, -1.0f),
			LifeTime(1200.0f),
			ModelKey("ghost_FINAL")
		);

		PlaySound(LoadSound("Assets/Sound/spawn2.mp3"));
	}
	break;

	case SPAWN_STANDBY_LEFT_1:
	{
		Entity enemy = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(a_targetPos.x - 12.5f, a_targetPos.y + 5.5f, a_targetPos.z + 100.0f), float3(0.0f, 90.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
			BoxCollider(float3{ 0.0f,0.0f,0.0f }, float3{ 3.0f, 6.0f,3.0f }),
			OBBCollider(OBB_PushOutLocked),
			Ray(float3()),
			EnemyTag{},
			Velocity(),
			LifeTime(2100.0f),
			ModelKey("ghost_FINAL")
		);

		PlaySound(LoadSound("Assets/Sound/search2.mp3"));
	}
	break;

	case SPAWN_STANDBY_RIGHT_1:
	{
		Entity enemy = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(a_targetPos.x + 12.5f, a_targetPos.y + 5.5f, a_targetPos.z + 70.0f), float3(0.0f, -90.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
			BoxCollider(float3{ 0.0f,0.0f,0.0f }, float3{ 3.0f, 6.0f,3.0f }),
			OBBCollider(OBB_PushOutLocked),
			Ray(float3()),
			EnemyTag{},
			Velocity(),
			LifeTime(2100.0f),
			ModelKey("ghost_FINAL")
		);

		PlaySound(LoadSound("Assets/Sound/search2.mp3"));
	}
	break;

	case SPAWN_STANDBY_UP_1:
	{
		Entity enemy = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(a_targetPos.x, a_targetPos.y + 20.5f, a_targetPos.z + 100.0f), float3(90.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
			BoxCollider(float3{ 0.0f,0.0f,0.0f }, float3{ 3.0f, 6.0f,3.0f }),
			OBBCollider(OBB_PushOutLocked),
			Ray(float3()),
			EnemyTag{},
			Velocity(),
			LifeTime(1200.0f),
			ModelKey("ghost_FINAL"),
			DeadState()
		);

		PlaySound(LoadSound("Assets/Sound/search2.mp3"));
	}
	break;

	case SPAWN_SHOOTER_1:
	{
		SpawnShooterEnemy(a_chunk, a_targetPos);
	}
	break;
	case SPAWN_GROUP_1:
	{
		SpawnGroupEnemy(a_chunk, a_targetPos, float3(0.3f, 0.1f, 0.9f), 3);
	}

	}


}

void SpawnNormalEnemy(Chunk& a_chunk, float3 a_targetPos, float3 a_spawnRailPosNormalized)
{
	float3 random = RandomEnemyPick(kEnemyRandomWidth, kEnemyRandomHeight, kEnemyRandomDepth);
	float3 railPos(
		a_spawnRailPosNormalized.x * kEnemySpawnRateWidth + random.x,
		a_spawnRailPosNormalized.y * kEnemySpawnRateHeight + random.y,
		a_spawnRailPosNormalized.z * kEnemySpawnRateDepth + random.z);
	Entity player = GetPlayer(a_chunk, a_targetPos);
	Entity rail = GetRail(a_chunk);

	Entity enemy = a_chunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(railPos, float3(0.0f, 0.0f, 0.0f), float3(kEnemyNormalScale, kEnemyNormalScale, kEnemyNormalScale)),
		BoxCollider(float3{ 0.0f,1.0f,0.0f }, float3{ 3.0f, 6.0f,3.0f }),
		OBBCollider(OBB_PushOutLocked),
		EnemyTag{},
		LifeTime(1200.0f),
		ModelKey("ghost_FINAL"),
		HitInfomation(),
		HitPoint(kEnemyMaxHP),
		AttackHitRecord(),
		PlayerViewTag{},
		PosePosState(POSE_POS_RAIL),
		PoseRotState(POSE_ROT_LOOK),
		RailUser(rail),
		RailFly(kEnemyFlyPower, 0.05f),
		LookComponent(player),
		MoveForward(kEnemyMoveSpeed),
		DeadState(),
		AddDamageComponent(kEnemyAttackDamage)
	);

	PlaySound(LoadSound("Assets/Sound/spawn1.mp3"));
}

void SpawnShooterEnemy(Chunk& a_chunk, float3 a_targetPos)
{
	float3 random = RandomEnemyPick(kEnemyRandomWidth, kEnemyRandomHeight, kEnemyRandomDepth);
	float3 railPos(random.x, random.y, 0.9f * kEnemySpawnRateDepth + random.z);
	Entity player = GetPlayer(a_chunk, a_targetPos);
	Entity rail = GetRail(a_chunk);

	Entity enemy = a_chunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(railPos, float3(0.0f, 0.0f, 0.0f), float3(kEnemyShooterScale.x, kEnemyShooterScale.y, kEnemyShooterScale.z)),
		BoxCollider(float3{ 0.0f,1.0f,0.0f }, float3{ 3.0f, 6.0f,3.0f }),
		OBBCollider(OBB_PushOutLocked),
		EnemyTag{},
		LifeTime(1200.0f),
		ModelKey("ghost_FINAL"),
		HitInfomation(),
		HitPoint(kEnemyMaxHP),
		AttackHitRecord(),
		PlayerViewTag{},
		PosePosState(POSE_POS_RAIL),
		PoseRotState(POSE_ROT_LOOK),
		RailUser(rail),
		EnemyShooter(300.0f),
		LookComponent(player),
		DeadState(),
		AddDamageComponent(kEnemyAttackDamage)
	);

	PlaySound(LoadSound("Assets/Sound/spawn1.mp3"));
}

void SpawnGroupEnemy(Chunk& a_chunk, float3 a_targetPos, float3 a_spawnRailPosNormalized, int a_groupEnemyCount)
{
	for(int i = 0; i < a_groupEnemyCount; i++)
	{
		float3 random = RandomEnemyPick(kEnemyRandomWidth, kEnemyRandomHeight, kEnemyRandomDepth);
		float3 railPos(
			a_spawnRailPosNormalized.x * kEnemySpawnRateWidth + random.x,
			a_spawnRailPosNormalized.y * kEnemySpawnRateHeight + random.y,
			a_spawnRailPosNormalized.z * kEnemySpawnRateDepth + random.z);
		Entity player = GetPlayer(a_chunk, a_targetPos);
		Entity rail = GetRail(a_chunk);

		Entity enemy = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(railPos, float3(0.0f, 0.0f, 0.0f), float3(kEnemyNormalScale, kEnemyNormalScale, kEnemyNormalScale)),
			BoxCollider(float3{ 0.0f,1.0f,0.0f }, float3{ 3.0f, 6.0f,3.0f }),
			OBBCollider(OBB_PushOutLocked),
			EnemyTag{},
			LifeTime(1200.0f),
			ModelKey("ghost_FINAL"),
			HitInfomation(),
			HitPoint(kEnemyMaxHP),
			AttackHitRecord(),
			PlayerViewTag{},
			PosePosState(POSE_POS_RAIL),
			PoseRotState(POSE_ROT_LOOK),
			RailUser(rail),
			RailFly(kEnemyFlyPower, 0.05f),
			LookComponent(player),
			MoveForward(kEnemyMoveSpeed),
			DeadState(),
			AddDamageComponent(kEnemyAttackDamage)
		);
	}

	PlaySound(LoadSound("Assets/Sound/spawn1.mp3"));
}

float3 RandomEnemyPick(int randomWidth, int randomHeight, int randomDepth)
{
	float3 ret;
	ret.x = (float)(rand() % randomWidth) - (randomWidth - 1.0f) / 2.0f;
	ret.y = (float)(rand() % randomHeight) - (randomHeight - 1.0f) / 2.0f;
	ret.z = (float)(rand() % randomDepth) - (randomDepth - 1.0f) / 2.0f;
	ret = ret * kEnemyRandomRate;
	return ret;
}

bool SpawnEnemyBullet(Chunk& a_chunk, float3 a_shooterOffset, Entity a_player)
{
	Entity rail = GetRail(a_chunk);
	ComponentHandle<RailComponent> railComp = a_chunk.GetComponent<RailComponent>(rail);
	Entity player = GetPlayer(a_chunk, railComp.Look().currentPos);
	a_chunk.CreateNewEntity(
		EnemyBulletTag(),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(a_shooterOffset),
			float3(0.0f, 0.0f, 0.0f),
			float3(2.5f, 2.5f, 2.5f)
		),
		PosePosState(POSE_POS_RAIL),
		PoseRotState(POSE_ROT_LOOK),
		BoxCollider(float3{ 0.0f, 0.0f, 0.0f }, float3{ 3.5f,3.5f,3.5f }),
		OBBCollider(),
		RailUser(rail),
		LookComponent(player),
		MoveForward(kEnemyShotSpeed),
		ModelKey("bolita_pinchos_pintada", float3(0.0f, -2.5f, 0.0f), float3()),
		GhostAreaComponent(5.0f),
		DeadState(),
		AddDamageComponent(kEnemyShotDamage)
	);

	return false;
}




