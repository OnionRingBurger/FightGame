#include "CharacterSystem.h"

using namespace Component;

void CharacterKill(Chunk& a_chunk, Entity killEntity);

namespace
{
	constexpr float kGroundNormalThreshold = 0.5f;
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
			}

			const ComponentHandle<JumpAction> jump = a_chunk.GetComponent<JumpAction>(it);
			if (jump.IsValid())
			{
				allowed &= ~ActionFlag_Jump;
			}

			// !!!New!!!
			const ComponentHandle<AttackAction> attackAction = a_chunk.GetComponent<AttackAction>(it);
			if (attackAction.IsValid())
			{
				allowed &= ~ActionFlag_Attack;
			}

			const ComponentHandle<AttackWaitAction> attackWait = a_chunk.GetComponent<AttackWaitAction>(it);
			if (attackWait.IsValid())
			{
				allowed &= ~ActionFlag_Move;
				allowed &= ~ActionFlag_Jump;
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
		// TODO 必要に応じて後隙は追加する
		// 何らかの方法で攻撃判定が消滅した場合攻撃終了する
		Entity attackEntity = attackAction.Look().attackEntity;
		// 攻撃判定が残っている場合削除
		const bool hasPlayerAttack = a_chunk.GetComponent<PlayerAttackTag>(attackEntity).IsValid();
		const bool hasEnemyAttack = a_chunk.GetComponent<EnemyAttackTag>(attackEntity).IsValid();
		if (!hasPlayerAttack && !hasEnemyAttack)
		{
			a_chunk.DeleteChunkComponent(it, AttackAction::kTypeId);
			if (a_chunk.GetComponent<AttackWaitAction>(it).IsValid())
			{
				a_chunk.DeleteChunkComponent(it, AttackWaitAction::kTypeId);
			}
		}
	}
}

// !!!New!!!
void AttackWaitActionSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<AttackWaitAction, AttackPower>>();

	for (auto it : view)
	{
		ComponentHandle<AttackWaitAction> attackWait = a_chunk.GetComponent<AttackWaitAction>(it);
		const ComponentHandle<AttackPower> attackPower = a_chunk.GetComponent<AttackPower>(it);

		attackWait->elapsedTime += a_context.deltaTime;
		if (attackWait.Look().elapsedTime >= attackPower.Look().waitTime)
		{
			a_chunk.DeleteChunkComponent(it, AttackWaitAction::kTypeId);
		}
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
