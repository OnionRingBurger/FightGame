#include "PlayerSystem.h"
#include "Components.h"
#include "SystemAssist.h"
#include "Sound.h"
#include "Defines.h"
#include "GameData.h"

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
	ComponentView view = a_chunk.GetView<ComponentTypes<MoveInputResult, ActionMask, JumpPower>>();

	for (auto it : view)
	{
		if (!IsActionAllowed(a_chunk, it, ActionFlag_Jump)) continue;

		const ComponentHandle<JumpAction> jump = a_chunk.GetComponent<JumpAction>(it);
		if (jump.IsValid()) continue;

		const ComponentHandle<MoveInputResult> result = a_chunk.GetComponent<MoveInputResult>(it);
		if (!result.Look().useJump) continue;

		// 攻撃中だった場合キャンセルする
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

		// タイトル画面じゃなかったら音を出す
		ComponentHandle<InputSource> source = a_chunk.GetComponent<InputSource>(it);
		if(source.Look().jump != InputOrigin::Title) PlaySound(LoadSound(path.c_str()));
	}
}

void PlayerHealSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<PlayerHeal>>();

	Entity player = GetPlayer(a_chunk, float3());
	ComponentHandle<HitPoint> hp = a_chunk.GetComponent<HitPoint>(player);

	for (auto it : view)
	{
		// HPを回復する
		ComponentHandle<PlayerHeal> heal = a_chunk.GetComponent<PlayerHeal>(it);
		float healedHP = hp.Look().currentHP + heal.Look().healValue;
		hp->currentHP = std::min(healedHP, hp.Look().maxHP);
		// 回復Effectを生成
		a_chunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(),
				float3(),
				float3(1.0f, 1.0f, 1.0f)
			),
			FollowPosition(float3(), player),
			PosePosState(POSE_POS_FOLLOW),
			EfkEffectKey(kPlayerHealKey, false)
		);

		// 回復Entityを削除
		a_chunk.DeleteChunkEntity(it);
	}
}
