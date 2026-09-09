#include "PlayerSystem.h"
#include "Components.h"
#include "SystemAssist.h"
#include "Sound.h"
#include "Defines.h"

using namespace Component;


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



		float3 shakePower = float3(0.02f, 0.02f, 0.02f);
		float3 shakeAmp = float3(2.5f, 2.5f, 2.5f);
		float shakeTime = 130.0f;
		
		AddShakeEffect(a_chunk, it, shakePower, shakeAmp, shakeTime, 0.5f);


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
	ComponentView view = a_chunk.GetView<ComponentTypes<PlayerTag, MoveInputResult, ActionMask, JumpPower>>();

	for (auto it : view)
	{
		if (!IsActionAllowed(a_chunk, it, ActionFlag_Jump)) continue;

		const ComponentHandle<JumpAction> jump = a_chunk.GetComponent<JumpAction>(it);
		if (jump.IsValid()) continue;

		const ComponentHandle<MoveInputResult> result = a_chunk.GetComponent<MoveInputResult>(it);
		if (!result.Look().useJump) continue;

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

		std::string path = kSoundAssetPath;
		path += "jump.mp3";

		PlaySound(LoadSound(path.c_str()));
	}
}
