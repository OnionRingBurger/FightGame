#include "CharacterSystem.h"

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
				a_attackPower.maxLowness
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
		if (dead.IsValid() && dead.Look().isDead)
		{
			allowed = 0;
		}
		else
		{
			const ComponentHandle<GroundedState> grounded = a_chunk.GetComponent<GroundedState>(it);
			if (grounded.IsValid() && !grounded.Look().isGrounded)
			{
				allowed &= ~ActionFlag_Jump;
				allowed &= ~ActionFlag_Guard;
			}

			const ComponentHandle<JumpAction> jump = a_chunk.GetComponent<JumpAction>(it);
			if (jump.IsValid())
			{
				allowed &= ~ActionFlag_Jump;
				allowed &= ~ActionFlag_Guard;
			}

			// !!!New!!!
			const ComponentHandle<AttackAction> attackAction = a_chunk.GetComponent<AttackAction>(it);
			if (attackAction.IsValid())
			{
				allowed &= ~ActionFlag_Attack;
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
		
		// ガード不可だった場合抜ける
		if (!result.Look().useGuard || !IsActionAllowed(a_chunk, it, ActionFlag_Guard))
		{
			continue;
		}
		// 攻撃中だった場合キャンセルする
		CancelPlayerAttackIfAble(a_chunk, it);
		// ガードへ移行
		a_chunk.AddComponent(it, GuardAction(guardState.Look().guardPower));
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
