#include "CharacterSystem.h"

#include "GameData.h"
#include "Geometory.h"
#include "Sound.h"

using namespace Component;

void CharacterKill(Chunk& a_chunk, Entity killEntity);

namespace
{
	constexpr float kGroundNormalThreshold = 0.5f;

	// 攻撃Entity生成関数
	Entity SpawnActiveAttackEntity(Chunk& a_chunk, Entity a_attacker, const AttackPower& a_attackPower, int a_attackIndex)
	{
		const ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(a_attacker);
		if (!rotation.IsValid()) return kInvalidEntity;

		const bool isPlayer = a_chunk.GetComponent<PlayerTag>(a_attacker).IsValid();
		const bool isEnemy = a_chunk.GetComponent<EnemyTag>(a_attacker).IsValid();
		if (!isPlayer && !isEnemy) return kInvalidEntity;

		// 攻撃Entity生成
		Entity attack = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, rotation.Look().yaw, 0.0f),
				float3(1.0f, 1.0f, 1.0f)
			),
			SectorHitJudge(
				a_attackPower.minLength,
				a_attackPower.maxLength,
				a_attackPower.angle,
				a_attackPower.maxHeight,
				a_attackPower.maxLowness
			),
			AddDamageComponent(a_attackPower.damageValue),
			FollowPosition(a_attackPower.followOffset, a_attacker, FOLLOW_POS_LOCALOFFSET),
			PosePosState(POSE_POS_FOLLOW),
			AttackInstance(a_attacker, a_attackIndex, a_attackPower.lifeTime)
		);
	
		// 生成者に応じて攻撃タイプを生成
		if (isPlayer) a_chunk.AddComponent(attack, PlayerAttackTag());
		else if (isEnemy) a_chunk.AddComponent(attack, EnemyAttackTag());

		return attack;
	}

	// 判定表示を生成
	Entity SpawnAttackTelegraphEntity(Chunk& a_chunk, Entity a_attacker, const AttackPower& a_attackPower)
	{
		// 角度を取得
		const ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(a_attacker);
		if (!rotation.IsValid()) return kInvalidEntity;

		// !!!New!!!
		const std::string& sectorKey = a_attackPower.sectorKey;
		if (!sectorKey.empty())
		{
			Geometory::RegisterSector(
				sectorKey,
				a_attackPower.minLength,
				a_attackPower.maxLength,
				a_attackPower.angle,
				a_attackPower.maxHeight,
				a_attackPower.maxLowness,
				16);
		}

		// 向いている方向に判定表示を生成
		return a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, rotation.Look().yaw, 0.0f),
				float3(1.0f, 1.0f, 1.0f)
			),
			AttackTelegraph(
				a_attackPower.minLength,
				a_attackPower.maxLength,
				a_attackPower.angle,
				a_attackPower.maxHeight,
				a_attackPower.maxLowness,
				sectorKey
			),
			FollowPosition(a_attackPower.followOffset, a_attacker, FOLLOW_POS_LOCALOFFSET),
			PosePosState(POSE_POS_FOLLOW)
		);
	}

	// 攻撃開始
	void BeginActiveAttackFromPower(Chunk& a_chunk, Entity a_attacker, const AttackPower& a_attackPower, int a_attackIndex)
	{
		Entity attack = SpawnActiveAttackEntity(a_chunk, a_attacker, a_attackPower, a_attackIndex);
		if (attack == kInvalidEntity) return;

		PlaySound(LoadSound("Assets/Sound/attack.mp3"));

		// アクションを追加
		a_chunk.AddComponent(a_attacker, AttackAction(attack, 0.0f, a_attackIndex, a_attackPower.motionTime));
		if (a_attackPower.waitTime > 0.0f)
		{
			a_chunk.AddComponent(a_attacker, AttackWaitAction(0.0f, a_attackPower.waitTime));
		}
	}

	// 攻撃予測開始（攻撃前隙の表示用扇を生成し、AttackStartupAction を追加する）
	void BeginAttackTelegraphFromPower(
		Chunk& a_chunk,
		Entity a_attacker,
		const AttackPower& a_attackPower,
		int a_attackIndex)
	{
		// 攻撃予測Entityを生成
		Entity telegraph = SpawnAttackTelegraphEntity(a_chunk, a_attacker, a_attackPower);
		if (telegraph == kInvalidEntity) return;
		// 攻撃予測のActionStateを追加
		a_chunk.AddComponent(
			a_attacker,
			AttackStartupAction(a_attackPower.startupTime, a_attackIndex, telegraph));
	}

	// !!!New!!!
	float GetXZDistanceSq(const float3& a_from, const float3& a_to)
	{
		const float dx = a_to.x - a_from.x;
		const float dz = a_to.z - a_from.z;
		return dx * dx + dz * dz;
	}

	// !!!New!!!
	bool IsEnemyAlive(Chunk& a_chunk, Entity a_enemy)
	{
		const ComponentHandle<DeadState> deadState = a_chunk.GetComponent<DeadState>(a_enemy);
		return !deadState.IsValid() || !deadState.Look().isDead;
	}

	// !!!New!!!
	bool IsLookOnTargetValid(Chunk& a_chunk, Entity a_target)
	{
		if (a_target == kInvalidEntity) return false;
		if (!a_chunk.GetComponent<EnemyTag>(a_target).IsValid()) return false;
		const ComponentHandle<Position> targetPos = a_chunk.GetComponent<Position>(a_target);
		if (!targetPos.IsValid()) return false;
		return IsEnemyAlive(a_chunk, a_target);
	}

	// !!!New!!!
	float GetCameraHorizontalHalfAngleDeg(Chunk& a_chunk, Entity a_cameraEntity)
	{
		const ComponentHandle<Camera> camera = a_chunk.GetComponent<Camera>(a_cameraEntity);
		if (!camera.IsValid()) return 46.0f;

		const float halfVertRad = camera.Look().fovy * 0.5f * RAD;
		const float halfHorizRad = atanf(tanf(halfVertRad) * camera.Look().aspect);
		return halfHorizRad * DEG;
	}

	// 対象がカメラの描画範囲外かどうかを返す
	bool IsWithinCameraHorizontalFOV(
		Chunk& a_chunk,
		Entity a_cameraEntity,
		const float3& a_from,
		const float3& a_to,
		float a_halfAngleDeg)
	{
		const ComponentHandle<Rotation> cameraRot = a_chunk.GetComponent<Rotation>(a_cameraEntity);
		if (!cameraRot.IsValid()) return true;

		const float3 forward = GetForward(float3(0.0f, cameraRot.Look().yaw, 0.0f));
		float3 toTarget(a_to.x - a_from.x, 0.0f, a_to.z - a_from.z);
		const float length = NormalizeLength(toTarget.x, toTarget.y, toTarget.z);
		if (length < 1e-5f) return true;

		toTarget.x /= length;
		toTarget.z /= length;
		const float3 dir(toTarget.x, 0.0f, toTarget.z);
		const float cosHalfAngle = cosf(a_halfAngleDeg * RAD);
		return DotFloat3(forward, dir) >= cosHalfAngle;
	}

	// !!!New!!!
	float2 GetCameraRightXZ(Chunk& a_chunk, Entity a_cameraEntity)
	{
		const ComponentHandle<Rotation> cameraRot = a_chunk.GetComponent<Rotation>(a_cameraEntity);
		if (!cameraRot.IsValid()) return float2(1.0f, 0.0f);

		const float3 forward = GetForward(float3(0.0f, cameraRot.Look().yaw, 0.0f));
		return float2(forward.z, -forward.x);
	}

	// AIがアホ、コード嫁
	int ResolveLookOnSwitchDirection(const SystemContext& a_context)
	{
		if (a_context.input.IsRegisterTrigger("LookOnPrev")) return -1;
		if (a_context.input.IsRegisterTrigger("LookOnNext")) return 1;
		return 0;
	}

	// 敵が範囲内に存在するか探索する
	Entity FindNearestLookOnEnemy(
		Chunk& a_chunk,
		const float3& a_playerPos,
		float a_radiusSq,
		Entity a_cameraEntity,
		float a_halfFovDeg)
	{
		Entity nearestEnemy = kInvalidEntity;
		float nearestDistSq = FLT_MAX;

		ComponentView enemyView = a_chunk.GetView<ComponentTypes<EnemyTag, Position>>();
		for (auto enemyIt : enemyView)
		{
			if (!IsEnemyAlive(a_chunk, enemyIt)) continue;

			// 距離を比較して範囲外なら抜ける
			// AIがアホ、座標は確定しているためPoseを取得する必要性が皆無
			const float3 enemyPos = GetEntityWorldPos(a_chunk, enemyIt);
			const float distSq = GetXZDistanceSq(a_playerPos, enemyPos);
			if (distSq > a_radiusSq) continue;
			//　カメラの範囲外かも判定
			if (!IsWithinCameraHorizontalFOV(a_chunk, a_cameraEntity, a_playerPos, enemyPos, a_halfFovDeg)) continue;

			// 一番近かった場合記録する
			if (distSq < nearestDistSq)
			{
				nearestDistSq = distSq;
				nearestEnemy = enemyIt;
			}
		}

		return nearestEnemy;
	}

	// ロックオン切り替え、現在はカメラから見た方向で取得している
	Entity FindLookOnSwitchTarget(
		Chunk& a_chunk,
		const float3& a_playerPos,
		Entity a_currentTarget,
		int a_direction,
		float a_radiusSq,
		Entity a_cameraEntity)
	{
		if (a_direction == 0 || !IsLookOnTargetValid(a_chunk, a_currentTarget)) return kInvalidEntity;

		const float3 currentPos = GetEntityWorldPos(a_chunk, a_currentTarget);
		const float2 toCurrent(currentPos.x - a_playerPos.x, currentPos.z - a_playerPos.z);
		if (GetLengthSq(toCurrent) < 1e-8f) return kInvalidEntity;
		const float2 currentDir = Normalize(toCurrent);
		const float2 cameraRight = GetCameraRightXZ(a_chunk, a_cameraEntity);

		Entity bestEnemy = kInvalidEntity;
		float bestAngle = FLT_MAX;

		ComponentView enemyView = a_chunk.GetView<ComponentTypes<EnemyTag, Position>>();
		for (auto enemyIt : enemyView)
		{
			if (enemyIt == a_currentTarget) continue;
			if (!IsEnemyAlive(a_chunk, enemyIt)) continue;

			const float3 enemyPos = GetEntityWorldPos(a_chunk, enemyIt);
			const float distSq = GetXZDistanceSq(a_playerPos, enemyPos);
			if (distSq > a_radiusSq) continue;

			const float2 toEnemy(enemyPos.x - a_playerPos.x, enemyPos.z - a_playerPos.z);
			const float2 enemyDir = Normalize(toEnemy);
			const float sideDot = enemyDir.x * cameraRight.x + enemyDir.y * cameraRight.y;
			if (a_direction < 0 && sideDot >= 0.0f) continue;
			if (a_direction > 0 && sideDot <= 0.0f) continue;

			const float dirDot = currentDir.x * enemyDir.x + currentDir.y * enemyDir.y;
			const float clampedDot = std::clamp(dirDot, -1.0f, 1.0f);
			const float angle = acosf(clampedDot) * DEG;
			if (angle < bestAngle)
			{
				bestAngle = angle;
				bestEnemy = enemyIt;
			}
		}
		return bestEnemy;
	}

	void StateToMove(Chunk& a_chunk, Entity a_entity)
	{
		// Stateを変更
		ComponentHandle<PoseRotState> poseRot = a_chunk.GetComponent<PoseRotState>(a_entity);
		if (poseRot.IsValid()) poseRot->state = POSE_ROT_LOOKMOVE;
		// 固定座標を使わないためMotionに戻す
		ComponentHandle<MotionTransform> transform = a_chunk.GetComponent<MotionTransform>(a_entity);
		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(a_entity);
		transform->motionRot = result.Look().newRot;
	}

	void StateToLook(Chunk& a_chunk, Entity a_entity)
	{
		// ステートを変更
		ComponentHandle<PoseRotState> poseRot = a_chunk.GetComponent<PoseRotState>(a_entity);
		if (poseRot.IsValid()) poseRot->state = POSE_ROT_LOOK;
		else a_chunk.AddComponent<PoseRotState>(a_entity, PoseRotState(POSE_ROT_LOOK));
		// 座標を初期化
		ComponentHandle<MotionTransform> transform = a_chunk.GetComponent<MotionTransform>(a_entity);
		transform->motionRot = float3();
	}

	void CancelLookOn(Chunk& a_chunk, Entity a_user)
	{
		// Actionを削除
		a_chunk.DeleteChunkComponent(a_user, LookOnAction::kTypeId);
		// Stateを変更
		StateToMove(a_chunk, a_user);
		// マーカーの対象を更新
		Entity targetMarker = GetLookOnMarker(a_chunk);
		ComponentHandle<FollowPosition> markerFollow = a_chunk.GetComponent<FollowPosition>(targetMarker);
		if (markerFollow.IsValid()) markerFollow->targetEntity = kInvalidEntity;
		// カメラリグを更新
		Entity rig = GetCameraRig(a_chunk);
		a_chunk.DeleteChunkComponent(rig, PoseRotState::kTypeId);
		a_chunk.DeleteChunkComponent(rig, RigLookOn::kTypeId);
	}
}

void GroundedSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<GroundedState>>();

	for (auto it : view)
	{
		ComponentHandle<GroundedState> grounded = a_chunk.GetComponent<GroundedState>(it);
		bool isGrounded = false;

		const ComponentHandle<HitInfomation> info = a_chunk.GetComponent<HitInfomation>(it);
		if (info.IsValid())
		{
			for (const auto& hitIt : info.Look().hitResults)
			{
				if (hitIt.normal.y >= kGroundNormalThreshold)
				{
					isGrounded = true;
					break;
				}
			}
		}

		grounded->isGrounded = isGrounded;
	}
}

void CharacterActionMaskSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<ActionMask>>();

	for (auto it : view)
	{
		ComponentHandle<ActionMask> actionMask = a_chunk.GetComponent<ActionMask>(it);
		// 一度すべての行動があると仮定する
		BitFlag allowed = ActionFlag_All;
		// 各情報StateとActionStateから不可能な行動を算出し、フラグを降ろす
		const ComponentHandle<DeadState> dead = a_chunk.GetComponent<DeadState>(it);
		if ((dead.IsValid() && dead.Look().isDead))
		{
			allowed = 0;
		}
		else
		{
			const ComponentHandle<EntryAction> entry = a_chunk.GetComponent<EntryAction>(it);
			if(entry.IsValid())
			{
				allowed = 0 | ActionFlag_EntryDuration;
			}

			const ComponentHandle<GroundedState> grounded = a_chunk.GetComponent<GroundedState>(it);
			if (grounded.IsValid() && !grounded.Look().isGrounded)
			{
				allowed &= ~ActionFlag_Jump;
				allowed &= ~ActionFlag_Guard;
				allowed &= ~ActionFlag_EntryDuration;
			}

			const ComponentHandle<KnockbackAction> knockback = a_chunk.GetComponent<KnockbackAction>(it);
			if (knockback.IsValid())
			{
				// ノックバック中は新たにノックバックしない
				allowed &= ~ActionFlag_Jump;
				allowed &= ~ActionFlag_Guard;
				allowed &= ~ActionFlag_Move;
				allowed &= ~ActionFlag_Attack;
				allowed &= ~ActionFlag_KnockBack;
				allowed &= ~ActionFlag_RotChange;
			}

			const ComponentHandle<JumpAction> jump = a_chunk.GetComponent<JumpAction>(it);
			if (jump.IsValid())
			{
				allowed &= ~ActionFlag_Jump;
				allowed &= ~ActionFlag_Guard;
			}

			const ComponentHandle<LookOnAction> lookonAction = a_chunk.GetComponent<LookOnAction>(it);
			if (lookonAction.IsValid())
			{
				allowed &= ~ActionFlag_LookMove;
			}

			// !!!New!!!
			const ComponentHandle<AttackAction> attackAction = a_chunk.GetComponent<AttackAction>(it);
			if (attackAction.IsValid())
			{
				allowed &= ~ActionFlag_Attack;
				allowed &= ~ActionFlag_LookMove;
				allowed &= ~ActionFlag_RotChange;
			}

			// !!!New!!!
			const ComponentHandle<AttackStartupAction> attackStartup = a_chunk.GetComponent<AttackStartupAction>(it);
			if (attackStartup.IsValid())
			{
				allowed &= ~ActionFlag_Attack;
				allowed &= ~ActionFlag_Move;
				allowed &= ~ActionFlag_Jump;
				allowed &= ~ActionFlag_Aim;
				allowed &= ~ActionFlag_Guard;
				allowed &= ~ActionFlag_LookMove;
				allowed &= ~ActionFlag_RotChange;
			}

			const ComponentHandle<AttackWaitAction> attackWait = a_chunk.GetComponent<AttackWaitAction>(it);
			if (attackWait.IsValid())
			{
				allowed &= ~ActionFlag_Move;
				allowed &= ~ActionFlag_Jump;
				allowed &= ~ActionFlag_Guard;
			}

			const ComponentHandle<GuardAction> guard = a_chunk.GetComponent<GuardAction>(it);
			if(guard.IsValid())
			{
				allowed &= ~ActionFlag_Attack;
				allowed &= ~ActionFlag_Jump;
				allowed &= ~ActionFlag_Guard;
				allowed &= ~ActionFlag_Move;
			}


		}

		actionMask->allowed = allowed;
	}
}

void AttackActionSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<AttackAction>>();

	for (auto it : view)
	{
		ComponentHandle<AttackAction> attackAction = a_chunk.GetComponent<AttackAction>(it);
		attackAction->elapsedTime += a_context.deltaTime;
		// モーション終了した場合アクションを終了
		if (attackAction.Look().elapsedTime >= attackAction.Look().maxDuration)
		{
			a_chunk.DeleteChunkComponent(it, AttackAction::kTypeId);
			a_chunk.DeleteChunkComponent(it, AttackWaitAction::kTypeId);
		}
	}
}

// !!!New!!!
void AttackWaitActionSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<AttackWaitAction>>();

	for (auto it : view)
	{
		ComponentHandle<AttackWaitAction> attackWait = a_chunk.GetComponent<AttackWaitAction>(it);

		attackWait->elapsedTime += a_context.deltaTime;
		if (attackWait.Look().elapsedTime >= attackWait.Look().waitDuration)
		{
			a_chunk.DeleteChunkComponent(it, AttackWaitAction::kTypeId);
		}
	}
}

// 攻撃予測が終了した際、実際に攻撃を生成する
void AttackStartupSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<AttackStartupAction, AttackStatus>>();

	for (auto it : view)
	{
		ComponentHandle<AttackStartupAction> attackStartup = a_chunk.GetComponent<AttackStartupAction>(it);
		attackStartup->elapsedTime += a_context.deltaTime;
		if (attackStartup.Look().elapsedTime < attackStartup.Look().startupDuration)
		{
			continue;
		}

		const Entity telegraphEntity = attackStartup.Look().telegraphEntity;
		const int attackIndex = attackStartup.Look().attackIndex;

		if (telegraphEntity != kInvalidEntity
			&& a_chunk.GetComponent<AttackTelegraph>(telegraphEntity).IsValid())
		{
			a_chunk.DeleteChunkEntity(telegraphEntity);
		}

		// 攻撃状態へ移行する
		const ComponentHandle<AttackStatus> attackStatus = a_chunk.GetComponent<AttackStatus>(it);
		// 攻撃を生成
		if (attackIndex >= 0
			&& attackIndex < static_cast<int>(attackStatus.Look().attackPowers.size()))
		{
			const AttackPower& attackPower = attackStatus.Look().attackPowers[static_cast<size_t>(attackIndex)];
			BeginActiveAttackFromPower(a_chunk, it, attackPower, attackIndex);
		}

		// 攻撃待機を終了する
		a_chunk.DeleteChunkComponent(it, AttackStartupAction::kTypeId);
	}
}

// !!!New!!!
void CharacterAttackSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<MoveInputResult, Rotation, AttackStatus>>();

	for (auto it : view)
	{
		const ComponentHandle<MoveInputResult> inputResult = a_chunk.GetComponent<MoveInputResult>(it);
		if (!inputResult.Look().useAttack) continue;

		// 攻撃不可だった場合抜ける
		if (!IsActionAllowed(a_chunk, it, ActionFlag_Attack)) continue;

		// キャラクター以外の攻撃だったら抜ける
		const bool isPlayer = a_chunk.GetComponent<PlayerTag>(it).IsValid();
		const bool isEnemy = a_chunk.GetComponent<EnemyTag>(it).IsValid();
		if (!isPlayer && !isEnemy) continue;

		// 攻撃ステータスを取得
		const ComponentHandle<AttackStatus> attackStatus = a_chunk.GetComponent<AttackStatus>(it);
		const int attackIndex = inputResult.Look().attackIndex;
		// 存在しない場合抜ける
		if (attackIndex < 0 || attackIndex >= static_cast<int>(attackStatus.Look().attackPowers.size()))
		{
			continue;
		}
		const AttackPower& attackPower = attackStatus.Look().attackPowers[static_cast<size_t>(attackIndex)];

		// 攻撃前のタメがある場合判定表示を生成
		if (attackPower.startupTime > 0.0f)
		{
			BeginAttackTelegraphFromPower(a_chunk, it, attackPower, attackIndex);
			if (isPlayer) PlaySound(LoadSound("Assets/Sound/startup.mp3"));
			else PlaySound(LoadSound("Assets/Sound/enemystartup.mp3"));

			continue;
		}
		// ない場合攻撃を生成
		BeginActiveAttackFromPower(a_chunk, it, attackPower, attackIndex);

	}
}


// !!!New!!!
void AttackInstanceEndCheckSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<AttackInstance>>();

	for (auto it : view)
	{
		// 終了条件を更新する
		ComponentHandle<AttackInstance> attackInstance = a_chunk.GetComponent<AttackInstance>(it);
		// 終了していた場合更新する必要がないので抜ける
		if (attackInstance.Look().ended)
		{
			continue;
		}

		// 生存時間を超えていたら終了
		attackInstance->elapsedTime += a_context.deltaTime;
		if (attackInstance.Look().elapsedTime >= attackInstance.Look().maxDuration)
		{
			attackInstance->ended = true;
			continue;
		}

		// 親が存在していなかったら終了
		const Entity owner = attackInstance.Look().owner;
		if (owner == kInvalidEntity)
		{
			attackInstance->ended = true;
			continue;
		}

		// 親が死んでいたら終了
		const ComponentHandle<DeadState> ownerDead = a_chunk.GetComponent<DeadState>(owner);
		if (ownerDead.IsValid() && ownerDead.Look().isDead)
		{
			attackInstance->ended = true;
			continue;
		}

		// 親が攻撃モーション注出なかった場合フラグに応じて終了
		if (attackInstance.Look().endWithOwnerAction)
		{
			const ComponentHandle<AttackAction> ownerAction = a_chunk.GetComponent<AttackAction>(owner);
			if (!ownerAction.IsValid() || ownerAction.Look().attackEntity != it)
			{
				attackInstance->ended = true;
				continue;
			}
		}
	}
}

// !!!New!!!
void AttackInstanceResolveSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager)
{
	(void)a_context;

	ComponentView view = a_chunk.GetView<ComponentTypes<AttackInstance>>();

	for (auto it : view)
	{
		const ComponentHandle<AttackInstance> attackInstance = a_chunk.GetComponent<AttackInstance>(it);
		if (!attackInstance.Look().ended)
		{
			continue;
		}

		a_aiManager.NotifyAttackResolved(
			attackInstance.Look().owner,
			attackInstance.Look().attackIndex,
			attackInstance.Look().connected);

		a_chunk.DeleteChunkEntity(it);
	}
}

void JumpPhysicsSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<JumpAction>>();

	for (auto it : view)
	{
		const ComponentHandle<GroundedState> grounded = a_chunk.GetComponent<GroundedState>(it);
		ComponentHandle<Velocity> velocity = a_chunk.GetComponent<Velocity>(it);

		if (!velocity.IsValid()) continue;

		if (grounded.IsValid() && grounded.Look().isGrounded && velocity.Look().y <= 0.0f)
		{
			velocity->y = 0.0f;
			a_chunk.DeleteChunkComponent(it, JumpAction::kTypeId);
		}
	}
}

void HitPointSystem(Chunk& a_chunk, SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<HitPoint>>();

	for (auto it : view)
	{
		ComponentHandle<HitPoint> hp = a_chunk.GetComponent<HitPoint>(it);

		if (hp.Look().currentHP <= 0.0f) CharacterKill(a_chunk, it);
	}
}


void CharacterKill(Chunk& a_chunk, Entity killEntity)
{
	ComponentHandle<DeadState> deadState = a_chunk.GetComponent<DeadState>(killEntity);
	if (deadState.IsValid()) deadState->isDead = true;
}

void GuardSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<GuardState, MoveInputResult>>();
	for (auto it : view)
	{
		ComponentHandle<MoveInputResult> result = a_chunk.GetComponent<MoveInputResult>(it);
		ComponentHandle<GuardState> guardState = a_chunk.GetComponent<GuardState>(it);

		DebugConsole::SetDrawPos(15, 19);
		std::cout << "Input = " << a_context.input.IsRegisterPress("Guard") << std::endl;
		DebugConsole::SetDrawPos(15, 20);
		std::cout << "GuardSystem: useGuard = " << result.Look().useGuard << std::endl;

		// ガード不可だった場合抜ける
		if (!result.Look().useGuard || !IsActionAllowed(a_chunk, it, ActionFlag_Guard))
		{
			continue;
		}
		// 攻撃中だった場合キャンセルする
		CancelPlayerAttackIfAble(a_chunk, it);
		// ガードへ移行
		a_chunk.AddComponent(it, GuardAction(guardState.Look().guardPower, guardState.Look().knockbackRate));
	}
}

void GuardActionSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<GuardState, GuardAction, MoveInputResult>>();
	for (auto it : view)
	{
		ComponentHandle<MoveInputResult> result = a_chunk.GetComponent<MoveInputResult>(it);
		ComponentHandle<GuardAction> guard = a_chunk.GetComponent<GuardAction>(it);

		// ガード中だった場合解除しない
		if (result.Look().useGuard) continue;
		
		a_chunk.DeleteChunkComponent(it, GuardAction::kTypeId);
	
	}
}

// !!!New!!!
void LookOnStateSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	const Entity cameraEntity = GetCamera(a_chunk);
	const float halfFovDeg = GetCameraHorizontalHalfAngleDeg(a_chunk, cameraEntity);
	const int switchDirection = ResolveLookOnSwitchDirection(a_context);
	

	// ロックオン継続管理
	ComponentView actionView = a_chunk.GetView<ComponentTypes<LookOnState, LookOnAction>>();
	for (auto playerIt : actionView)
	{
		ComponentHandle<LookOnAction> action = a_chunk.GetComponent<LookOnAction>(playerIt);
		ComponentHandle<LookOnState> state = a_chunk.GetComponent<LookOnState>(playerIt);

		
		// 死亡込みでロックオン対象がいるか確認する
		if (!IsLookOnTargetValid(a_chunk, action.Look().target))
		{
			CancelLookOn(a_chunk, playerIt);
			continue;
		}


		// 対象への距離と座標を取得
		const float3 playerPos = GetEntityWorldPos(a_chunk, playerIt);
		const float releaseRadiusSq = state.Look().releaseRadius * state.Look().releaseRadius;
		const float3 targetPos = GetEntityWorldPos(a_chunk, action.Look().target);
		const float targetDistSq = GetXZDistanceSq(playerPos, targetPos);
		// AIがアホ、勝手に既存フローに乗っていない方法で入力を取ってる
		if (switchDirection != 0)
		{
			
			// 切り替え方向に対象が存在するか確認
			const Entity switchTarget = FindLookOnSwitchTarget(
				a_chunk,
				playerPos,
				action.Look().target,
				switchDirection,
				releaseRadiusSq,
				cameraEntity);
			// 対象が存在した場合代入する
			if (switchTarget != kInvalidEntity)
			{
				action->target = switchTarget;
				Entity targetMarker = GetLookOnMarker(a_chunk);
				ComponentHandle<FollowPosition> markerFollow = a_chunk.GetComponent<FollowPosition>(targetMarker);
				if (markerFollow.IsValid()) markerFollow->targetEntity = switchTarget;
			}
			continue;
		}

		// 対象が範囲外だった場合抜ける
		if (targetDistSq > releaseRadiusSq)
		{
			CancelLookOn(a_chunk, playerIt);
			continue;
		}
	}
	// 一番近い敵を登録
	ComponentView searchView = a_chunk.GetView<ComponentTypes<LookOnState>, ComponentTypes<LookOnAction>>();
	for (auto playerIt : searchView)
	{
		ComponentHandle<LookOnState> state = a_chunk.GetComponent<LookOnState>(playerIt);
		const float3 playerPos = GetEntityWorldPos(a_chunk, playerIt);
		const float captureRadiusSq = state.Look().captureRadius * state.Look().captureRadius;

		// 一番近い敵を取得
		const Entity nearestEnemy = FindNearestLookOnEnemy(
			a_chunk,
			playerPos,
			captureRadiusSq,
			cameraEntity,
			halfFovDeg);
		if (nearestEnemy == kInvalidEntity) continue;

		// 一番近い敵が範囲内だった場合Actionを追加し、登録する
		a_chunk.AddComponent(playerIt, LookOnAction(nearestEnemy));
		// Stateを変更
		StateToLook(a_chunk, playerIt);
		// マーカーを設定
		Entity targetMarker = GetLookOnMarker(a_chunk);
		ComponentHandle<FollowPosition> markerFollow = a_chunk.GetComponent<FollowPosition>(targetMarker);
		if (markerFollow.IsValid()) markerFollow->targetEntity = nearestEnemy;

		// Rigを設定
		Entity rig = GetCameraRig(a_chunk);
		ComponentHandle<PoseRotState> rigRotState = a_chunk.GetComponent<PoseRotState>(rig);
		if (rigRotState.IsValid())
		{
			rigRotState->state = POSE_ROT_LOOK;
		}
		else a_chunk.AddComponent(rig, PoseRotState(POSE_ROT_LOOK));
		a_chunk.AddComponent(rig, RigLookOn());

	}
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

void AttackHitSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_response, AIManager& a_aiManager)
{
	// HitInfomationを保持したEnemyを取得する
	ComponentView view = a_chunk.GetView<ComponentTypes<HitInfomation, HitPoint, AttackHitRecord>>();

	Entity camera = GetCamera(a_chunk);

	for (auto it : view)
	{
		// 敵のぶつかった結果を取得
		const ComponentHandle<HitInfomation> info = a_chunk.GetComponent<HitInfomation>(it);
		ComponentHandle<AttackHitRecord> hitRecord = a_chunk.GetComponent<AttackHitRecord>(it);
		const ComponentHandle<GuardAction> guardAction = a_chunk.GetComponent<GuardAction>(it);
		float guardPower = 1.0f;

		bool isPlayer = a_chunk.GetComponent<PlayerTag>(it).IsValid();
		bool isEnemy = a_chunk.GetComponent<EnemyTag>(it).IsValid();
		bool isGuard = guardAction.IsValid();

		if (isGuard)
		{
			guardPower = guardAction.Look().currentGuardPower;
		};

		// ノックバック倍率
		float knockBackRate = 1.0f;
		if(isGuard) knockBackRate *= guardAction.Look().knockbackRate;

		for (const auto& triggerIt : info.Look().triggerResults)
		{
			// 自身と同じタイプだった場合次ループへ移行
			if (isPlayer)
			{
				if (a_chunk.GetComponent<PlayerAttackTag>(triggerIt.triggerEntity).IsValid()) continue;
			}
			else if (isEnemy)
			{
				if (a_chunk.GetComponent<EnemyAttackTag>(triggerIt.triggerEntity).IsValid()) continue;
			}

			// ぶつかった対象ダメージを持っていなかった場合は次ループへ移行
			const ComponentHandle<AddDamageComponent> damage = a_chunk.GetComponent<AddDamageComponent>(triggerIt.triggerEntity);
			if (!damage.IsValid()) continue;

			// 攻撃が被弾クールタイムなら次ループへ移行

			auto coolDownIt = hitRecord->entries.find(triggerIt.triggerEntity);
			// クールタイムが残っている場合は次ループへ移行
			if (coolDownIt != hitRecord->entries.end() && coolDownIt->second > 0.0f)
			{
				continue;
			}

			// 体力にダメージを与える
			ComponentHandle<HitPoint> hitPoint = a_chunk.GetComponent<HitPoint>(it);
			hitPoint->currentHP -= damage.Look().damageValue * guardPower;


			// ノックバックを入力
			ComponentHandle<InterferenceResult> interferenceResult = a_chunk.GetComponent<InterferenceResult>(it);
			ComponentHandle<Position> thisPos = a_chunk.GetComponent<Position>(it);
			ComponentHandle<Position> triggerPos = a_chunk.GetComponent<Position>(triggerIt.triggerEntity);
			if (interferenceResult.IsValid() && thisPos.IsValid() && triggerPos.IsValid())
			{
				interferenceResult->useKnockback = true;
				float2 toTriggerVector = float2(triggerPos.Look().x - thisPos.Look().x, triggerPos.Look().z - thisPos.Look().z);
				float2 normalieVector = Normalize(float2(-toTriggerVector.x, -toTriggerVector.y));
				interferenceResult->knockBackDir = normalieVector;
				interferenceResult->knockbackTime = 5.0f * knockBackRate;
			}

			// 攻撃成功を保存
			ComponentHandle<AttackInstance> attackInstance =
				a_chunk.GetComponent<AttackInstance>(triggerIt.triggerEntity);
			if (attackInstance.IsValid())
			{
				attackInstance->connected = true;
			}

			if (isPlayer)
			{
				a_aiManager.NotifyHitResolved(triggerIt.triggerEntity);
			}

			// 画面エフェクトを出す
			float waitTime = isGuard ? 8.0f : 2.0f;
			a_response.AddStopTime(waitTime, 20.0f, 0.05f);
			float3 shakePower = isGuard ? float3(0.15f, 0.15f, 0.15f) : float3(0.2f, 0.2f, 0.2f);
			float3 shakeAmp = float3(0.1f, 0.1f, 0.1f);
			float shakeTime = isGuard ? 4.0f : 3.0f;
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
				a_chunk.AddComponent(camera, ShakeComponent(shakePower, shakeAmp, shakeTime));
			}

			// ダメージエフェクトを出す
			Entity hit = a_chunk.CreateNewEntity(
				TRANSFORM_COMPONENT(
					float3(thisPos.Look().x, thisPos.Look().y, thisPos.Look().z),
					float3(),
					float3(1.0f, 1.0f, 1.0f)
				),
				EfkEffectKey(kHitEffect, false)
				);

			if (isPlayer)
			{
				PlaySound(LoadSound("Assets/Sound/hit.mp3"));
			}
			else
			{
				PlaySound(LoadSound("Assets/Sound/enemyhit.mp3"));
			}
			// 被弾履歴に攻撃Entityとクールタイムを記録
			float cooldownDuration = kDefaultAttackHitCooldown;

			// 追加する
			hitRecord->entries.insert({ triggerIt.triggerEntity, cooldownDuration });

		}
	}

}

void AttackHitRecordCleanupSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// Viewを取得
	ComponentView view = a_chunk.GetView<ComponentTypes<AttackHitRecord>>();

	for (auto it : view)
	{
		ComponentHandle<AttackHitRecord> hitRecord = a_chunk.GetComponent<AttackHitRecord>(it);

		std::vector<Entity> removeEntities;

		for (auto entryIt = hitRecord->entries.begin(); entryIt != hitRecord->entries.end();)
		{
			// クールタイムを減らす
			entryIt->second -= a_context.deltaTime;

			// 被弾待機中でない場合対象をイテレータで削除する
			const ComponentHandle<PlayerAttackTag> playerAttackTag = a_chunk.GetComponent<PlayerAttackTag>(entryIt->first);
			const ComponentHandle<EnemyAttackTag> enemyAttackTag = a_chunk.GetComponent<EnemyAttackTag>(entryIt->first);

			bool hasAttackTag = playerAttackTag.IsValid() || enemyAttackTag.IsValid();
			if (entryIt->second <= 0.0f || !hasAttackTag)
			{
				// イテレータを削除して次のイテレータを取得
				entryIt = hitRecord->entries.erase(entryIt);
			}
			else
			{
				// 削除しない場合は次のイテレータへ
				++entryIt;
			}
		}
	}
}

void KnockbackStateSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<InterferenceResult>>();
	for (auto it : view)
	{
		if (!IsActionAllowed(a_chunk, it, ActionFlag_KnockBack)) continue;
		ComponentHandle<InterferenceResult> result = a_chunk.GetComponent<InterferenceResult>(it);
		if (!result->useKnockback) continue;

		a_chunk.AddComponent(it, KnockbackAction(result.Look().knockBackDir, 0.7f, result.Look().knockbackTime));
	}
}

// !!!New!!!
void KnockbackSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<Velocity, KnockbackAction>>();
	for (auto it : view)
	{
		ComponentHandle<Velocity> velocity = a_chunk.GetComponent<Velocity>(it);
		ComponentHandle<KnockbackAction> knockback = a_chunk.GetComponent<KnockbackAction>(it);
		velocity->x = knockback->moveDir.x * knockback->speed;
		velocity->z = knockback->moveDir.y * knockback->speed;

		knockback->elapsedTime += a_context.deltaTime;
		if (knockback->elapsedTime >= knockback->maxKnockbackTime)
		{
			a_chunk.DeleteChunkComponent(it, KnockbackAction::kTypeId);
			velocity->x = 0.0f;
			velocity->y = 0.0f;
		}
	}
}

void EntryActionSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<EntryAction>>();
	for (auto it : view)
	{
		if (!IsActionAllowed(a_chunk, it, ActionFlag_EntryDuration)) continue;

		ComponentHandle<EntryAction> action = a_chunk.GetComponent<EntryAction>(it);
		if (action.Look().canPlayEffect)
		{
			ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
			
			// エフェクトを生成
			a_chunk.CreateNewEntity(
				TRANSFORM_COMPONENT(
					float3(pos.Look().x, pos.Look().y - 0.5f, pos.Look().z),
					float3(),
					float3(1.0f, 1.0f, 1.0f)
				),
				EfkEffectKey(kEntryEffect, false)
			);
			action->canPlayEffect = false;

			PlaySound(LoadSound("Assets/Sound/entry.mp3"));

			// TODO カメラを揺らす
		}
		

		action->duration += a_context.deltaTime;
		if (action.Look().duration >= action.Look().maxDuration)
		{
			a_chunk.DeleteChunkComponent(it, EntryAction::kTypeId);
		}
	}

}
