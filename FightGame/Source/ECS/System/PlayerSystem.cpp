#include "PlayerSystem.h"
#include "Components.h"
#include "SystemAssist.h"
#include "Sound.h"

using namespace Component;

void PlayerWalkSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	//ComponentView view = a_chunk.GetView<ComponentTypes<PlayerTag, ShakingComponent, InputMove>>();

	//for (auto it : view)
	//{
	//	ComponentHandle<ShakingComponent> shaking = a_chunk.GetComponent<ShakingComponent>(it);
	//	const ComponentHandle<InputMove> inputMove = a_chunk.GetComponent<InputMove>(it);
	//	ComponentHandle<PlayerWalkTimer> timer = a_chunk.GetComponent<PlayerWalkTimer>(it);
	//	shaking->isWait = !inputMove.Look().isInput;
	//	if (timer.IsValid())
	//	{
	//		timer->time = std::clamp(timer.Look().time - a_context.deltaTime, 0.0f, timer.Look().maxTime);

	//		if ((inputMove.Look().isInput) &&
	//			shaking.Look().shankingDegree <= -1.0f &&
	//			timer.Look().time <= 0.0f)
	//		{
	//			timer->time = timer.Look().maxTime;
	//			PlaySound(LoadSound("Assets/Sound/walk.mp3"));
	//		}

	//	}

	//}
}

void PlayerDeadSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_response)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<PlayerTag, DeadState>>();
	for (auto it : view)
	{
		ComponentHandle<DeadState> dead = a_chunk.GetComponent<DeadState>(it);
		if (!dead.Look().isDead) continue;

		a_chunk.DeleteChunkComponent(it, PlayerTag::kTypeId);
		a_chunk.DeleteChunkComponent(it, InputMove::kTypeId);
		a_chunk.AddComponent(it, LifeTime(16.0f));
		//a_chunk.DeleteChunkComponent(it, InputRotato::kTypeId);
		//a_chunk.DeleteChunkComponent(it, Velocity::kTypeId);
		//a_chunk.DeleteChunkComponent(it, ShooterComponent::kTypeId);
		//a_chunk.DeleteChunkComponent(it, ShakingComponent::kTypeId);
		// a_chunk.DeleteChunkComponent(it, OBBCollider::kTypeId);
		//a_chunk.DeleteChunkComponent(it, RailFly::kTypeId);

		//ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent>>();
		//for (auto uiIt : view)
		//{
		//	a_chunk.DeleteChunkEntity(uiIt);
		//}



		float3 shakePower = float3(0.02f, 0.02f, 0.02f);
		float3 shakeAmp = float3(2.5f, 2.5f, 2.5f);
		float shakeTime = 130.0f;
		ComponentHandle<ShakeComponent> hitterShake = a_chunk.GetComponent<ShakeComponent>(it);
		if (hitterShake.IsValid())
		{
			hitterShake->shakePower = shakePower;
			hitterShake->shakeAmplitude = shakeAmp;
			hitterShake->elapsedTime = 0.0f;
			hitterShake->shakeTime = shakeTime;
		}
		else
		{
			a_chunk.AddComponent(it, ShakeComponent(shakePower, shakeAmp, shakeTime));
		}


		PlaySound(LoadSound("Assets/Sound/dead.mp3"));
		PlaySound(LoadSound("Assets/Sound/dead2.mp3"));
		PlaySound(LoadSound("Assets/Sound/enemydead.mp3"));

		a_response.AddStopTime(160.0f, 160.0f, 0.03f);

		//Entity createEffect = a_chunk.CreateNewEntity(
		//	CreateEffect(WHITEFADE_UP)
		//);

		//ComponentView gunView = a_chunk.GetView<ComponentTypes<GunTag, ModelKey>>();
		//// a_chunk.DeleteChunkComponent(it, DeadState::kTypeId);

		//for (auto gunIt : gunView)
		//{
		//	a_chunk.DeleteChunkComponent(gunIt, ModelKey::kTypeId);
		//}

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
