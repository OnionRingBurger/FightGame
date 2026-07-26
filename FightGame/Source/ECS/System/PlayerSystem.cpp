#include "PlayerSystem.h"
#include "Components.h"
#include "SystemAssist.h"
#include "Sound.h"

using namespace Component;

void PlayerWalkSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<PlayerTag, ShakingComponent, InputMove>>();

	for (auto it : view)
	{
		ComponentHandle<ShakingComponent> shaking = a_chunk.GetComponent<ShakingComponent>(it);
		const ComponentHandle<InputMove> inputMove = a_chunk.GetComponent<InputMove>(it);
		ComponentHandle<PlayerWalkTimer> timer = a_chunk.GetComponent<PlayerWalkTimer>(it);
		shaking->isWait = !inputMove.Look().isInput;
		if (timer.IsValid())
		{
			timer->time = std::clamp(timer.Look().time - a_context.deltaTime, 0.0f, timer.Look().maxTime);

			if ((inputMove.Look().isInput) &&
				shaking.Look().shankingDegree <= -1.0f &&
				timer.Look().time <= 0.0f)
			{
				timer->time = timer.Look().maxTime;
				PlaySound(LoadSound("Assets/Sound/walk.mp3"));
			}

		}

	}
}

void PlayerDeadSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<PlayerTag, DeadState>>();
	for (auto it : view)
	{
		ComponentHandle<DeadState> dead = a_chunk.GetComponent<DeadState>(it);
		if (!dead.Look().isDead) continue;

		a_chunk.DeleteChunkComponent(it, InputMove::kTypeId);
		a_chunk.DeleteChunkComponent(it, InputRotato::kTypeId);
		a_chunk.DeleteChunkComponent(it, Velocity::kTypeId);
		a_chunk.DeleteChunkComponent(it, ShooterComponent::kTypeId);
		a_chunk.DeleteChunkComponent(it, ShakingComponent::kTypeId);
		a_chunk.DeleteChunkComponent(it, OBBCollider::kTypeId);
		a_chunk.DeleteChunkComponent(it, RailFly::kTypeId);

		ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent>>();
		for (auto uiIt : view)
		{
			a_chunk.DeleteChunkEntity(uiIt);
		}

		Entity chunkChange = a_chunk.CreateNewEntity(
			ChunkChange(true, "Result"),
			DelayChunkChange(240.0f)
		);
		PlaySound(LoadSound("Assets/Sound/dead.mp3"));

		Entity createEffect = a_chunk.CreateNewEntity(
			CreateEffect(DARKFADE_UP)
		);

		ComponentView gunView = a_chunk.GetView<ComponentTypes<GunTag, ModelKey>>();
		a_chunk.DeleteChunkComponent(it, DeadState::kTypeId);

		for (auto gunIt : gunView)
		{
			a_chunk.DeleteChunkComponent(gunIt, ModelKey::kTypeId);
		}

	}
}

void PlayerAttackSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// çUåÇÉLÅ[Ç™âüÇ≥ÇÍÇƒÇ¢Ç»Ç©Ç¡ÇΩÇÁèCê≥
	if (!a_context.input.IsRegisterTrigger("Shot")) return;

	ComponentView view = a_chunk.GetView<ComponentTypes<PlayerTag, Position, Rotation, AttackPower>>();

	for (auto it : view)
	{
		if (!IsActionAllowed(a_chunk, it, ActionFlag_Attack)) continue;

		const ComponentHandle<AttackAction> attackAction = a_chunk.GetComponent<AttackAction>(it);
		if (attackAction.IsValid()) continue;

		const ComponentHandle<Position> position = a_chunk.GetComponent<Position>(it);
		const ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(it);
		const ComponentHandle<AttackPower> attackPower = a_chunk.GetComponent<AttackPower>(it);

		Entity attack = a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f), // FixedÇ≈ç¿ïWÇå≈íËÇ∑ÇÈÇΩÇﬂÅAèâä˙ç¿ïWÇÕì¸óÕÇµÇ»Ç¢ÅB
				float3(0.0f, rotation.Look().yaw, 0.0f),
				float3(1.0f, 1.0f, 1.0f)
			),
			SectorHitJudge(
				attackPower.Look().minLength,
				attackPower.Look().maxLength,
				attackPower.Look().angle,
				attackPower.Look().maxHeight,
				attackPower.Look().maxLowness
			),
			AddDamageComponent(attackPower.Look().damageValue),
			PlayerAttackTag(),
			LifeTime(attackPower.Look().lifeTime),
			FollowPosition(attackPower.Look().followOffset, it, FOLLOW_POS_LOCALOFFSET),
			PosePosState(POSE_POS_FOLLOW)
		);

		a_chunk.AddComponent(it, AttackAction(attack, 0.0f));
		// çUåÇçdíºéûä‘Ç™ë∂ç›Ç∑ÇÈèÍçáActionÇí«â¡Ç∑ÇÈ
		if (attackPower.Look().waitTime > 0.0f)
		{
			a_chunk.AddComponent(it, AttackWaitAction(0.0f));
		}
	}

}


void PlayerJumpSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	if (!a_context.input.IsRegisterTrigger("Jump")) return;

	ComponentView view = a_chunk.GetView<ComponentTypes<PlayerTag, ActionMask, JumpPower>>();

	for (auto it : view)
	{
		if (!IsActionAllowed(a_chunk, it, ActionFlag_Jump)) continue;

		const ComponentHandle<JumpAction> jump = a_chunk.GetComponent<JumpAction>(it);
		if (jump.IsValid()) continue;

		// çUåÇíÜÇæÇ¡ÇΩèÍçáÉLÉÉÉìÉZÉãÇ∑ÇÈ
		CancelPlayerAttackIfAble(a_chunk, it);

		const ComponentHandle<JumpPower> jumpPower = a_chunk.GetComponent<JumpPower>(it);
		a_chunk.AddComponent(it, JumpAction());

		ComponentHandle<Velocity> velocity = a_chunk.GetComponent<Velocity>(it);
		if (!velocity.IsValid())
		{
			a_chunk.AddComponent(it, Velocity());
			velocity = a_chunk.GetComponent<Velocity>(it);
		}
		velocity->x = jumpPower.Look().initialVelocity.x;
		velocity->y = jumpPower.Look().initialVelocity.y;
		velocity->z = jumpPower.Look().initialVelocity.z;
	}
}
