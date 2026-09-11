#include "GameLoopSystem.h"
#include "Components.h"
#include "Sound.h"
#include "SystemAssist.h"
#include "GameData.h"

using namespace Component;

void GoalSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<PlayerTag, HitInfomation>>();

	for (auto it : view)
	{
		ComponentHandle<HitInfomation> info = a_chunk.GetComponent<HitInfomation>(it);

		for (const auto& hitIt : info.Look().triggerResults)
		{
			ComponentHandle<GoalTag> goalTag = a_chunk.GetComponent<GoalTag>(hitIt.triggerEntity);

			if (goalTag.IsValid())
			{
				a_chunk.DeleteChunkComponent(it, InputMove::kTypeId);
				a_chunk.DeleteChunkComponent(it, InputRotato::kTypeId);
				a_chunk.DeleteChunkComponent(it, Velocity::kTypeId);
				a_chunk.DeleteChunkComponent(it, ShooterComponent::kTypeId);
				a_chunk.DeleteChunkComponent(it, ShakingComponent::kTypeId);
				a_chunk.DeleteChunkComponent(it, OBBCollider::kTypeId);
				a_chunk.DeleteChunkComponent(hitIt.triggerEntity, Velocity::kTypeId);

				ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent>>();
				for (auto it : view)
				{
					a_chunk.DeleteChunkEntity(it);
				}

				a_chunk.AddComponent(it, FollowRotation(0.0f, 180.0f, 0.0f, hitIt.triggerEntity));

				Entity gameOverUI = a_chunk.CreateNewEntity();

				Entity chunkChange = a_chunk.CreateNewEntity(
					ChunkChange(true, "Clear"),
					DelayChunkChange(240.0f)
				);

				Entity createEffect = a_chunk.CreateNewEntity(
					CreateEffect(WHITEFADE_UP)
				);

			}
		}
	}
}


void ChunkChangeProceed(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_chunkRequest, Entity a_changeEntity)
{
	ComponentHandle<ChunkChange> chunkChange = a_chunk.GetComponent<ChunkChange>(a_changeEntity);
	const ComponentHandle<KeyChunkChange> keyChunkChange = a_chunk.GetComponent<KeyChunkChange>(a_changeEntity);
	ComponentHandle<DelayChunkChange> delayChunkChange = a_chunk.GetComponent<DelayChunkChange>(a_changeEntity);


	if (keyChunkChange.IsValid() && a_context.input.IsRegisterTrigger(keyChunkChange.Look().key))
	{
		if (!delayChunkChange.IsValid()) a_chunk.AddComponent(a_changeEntity, DelayChunkChange(keyChunkChange.Look().waitTime));
		a_chunk.AddComponent(a_changeEntity, OtherChunkChangeLock());
	}

	if (delayChunkChange.IsValid())
	{
		delayChunkChange->currentCount -= a_context.deltaTime;
		if (delayChunkChange->currentCount < 0.0f)
		{
			chunkChange->isWait = false;
		}
	}

	if (!chunkChange->isWait)
	{
		a_chunkRequest.CreateWorldRequest(chunkChange.Look().chunkType);
	}
}

void ChunkChangeSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_chunkRequest)
{

	ComponentView lockView = a_chunk.GetView<ComponentTypes<ChunkChange, OtherChunkChangeLock>>();
	for (auto it : lockView)
	{
		ChunkChangeProceed(a_chunk, a_context, a_chunkRequest, it);
		return;
	}

	ComponentView view = a_chunk.GetView<ComponentTypes<ChunkChange>>();

	for (auto it : view)
	{
		ChunkChangeProceed(a_chunk, a_context, a_chunkRequest, it);
	}
}

void CheckAliveTargetGameOverSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	ComponentView targetView = a_chunk.GetView<ComponentTypes<GameOverTarget>>();
	for (auto it : targetView)
	{
		// 対象が存在した場合関数を終了する
		return;
	}

	// 既にチャンク変更待ちだった場合何もせず抜ける
	ComponentView chunkChangeView = a_chunk.GetView<ComponentTypes<KeyChunkChange>>();
	for (auto it : chunkChangeView)
	{
		return;
	}

	Entity back = a_chunk.CreateNewEntity(
		UIComponent("RetryBack", float2(), float2(2.0f, 2.0f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.05f)
	);

	Entity backShadow = a_chunk.CreateNewEntity(
		UIComponent("BackShadow", float2(0.0f, 0.3f), float2(3.35f, 3.92f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.035f / 7.0f),
		FadeChange(FADE_CHANGE_FLICKER)
	);

	Entity retry = a_chunk.CreateNewEntity(
		UIComponent("Retry", float2(0.0f, 0.3f), float2(1.15f, 0.8f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.035f)
	);



	Entity buttonA = a_chunk.CreateNewEntity(
		UIComponent("ButtonA", float2(-0.55f, -0.8f), float2(0.1f, 0.2f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.035f, 0.3f, 1.0f),
		FadeChange(FADE_CHANGE_FLICKER)
	);


	Entity buttonB = a_chunk.CreateNewEntity(
		UIComponent("ButtonB", float2(0.2f, -0.8f), float2(0.1f, 0.2f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.035f, 0.3f, 1.0f),
		FadeChange(FADE_CHANGE_FLICKER)
	);

	Entity yes = a_chunk.CreateNewEntity(
		UIComponent("Yes", float2(-0.35f, -0.8f), float2(0.2f, 0.3f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.035f, 0.3f, 1.0f),
		FadeChange(FADE_CHANGE_FLICKER)
	);


	Entity no = a_chunk.CreateNewEntity(
		UIComponent("No", float2(0.4f, -0.8f), float2(0.15f, 0.2f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.035f, 0.3f, 1.0f),
		FadeChange(FADE_CHANGE_FLICKER)
	);

	Entity stopper = a_chunk.CreateNewEntity(
		AllActionStopper()
	);



	Entity chunkChangeButton = a_chunk.CreateNewEntity(
		ChunkChange(true, "Result"),
		KeyChunkChange("GameOver", 160.0f)
	);

	Entity chunkChangeButton2 = a_chunk.CreateNewEntity(
		ChunkChange(true, "Proto"),
		KeyChunkChange("Restart", 160.0f)
	);

	Entity effect = a_chunk.CreateNewEntity(
		EffectKey(DARKFADE_UP, "GameOver")
	);

	Entity effect2 = a_chunk.CreateNewEntity(
		EffectKey(WHITEMINIFADE_UP, "Restart")
	);

	//Entity chunkChange = a_chunk.CreateNewEntity(
	//	ChunkChange(true, "Result"),
	//	DelayChunkChange(600.0f)
	//);
}

void CheckAliveTargetEnemySystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	
	ComponentView enemyView = a_chunk.GetView<ComponentTypes<ClearTarget>>();
	for(auto it : enemyView)
	{
		// クリア対象が存在した場合関数を終了する
		return;
	}
	// まだ敵が残っている場合生成する
	bool spawn = false;
	ComponentView spawnerView = a_chunk.GetView<ComponentTypes<PhaseSpawner>>();
	for (auto it : spawnerView)	
	{
		ComponentHandle<PhaseSpawner> spawner = a_chunk.GetComponent<PhaseSpawner>(it);
		if (spawner.Look().spawnName.empty()) continue;
		spawn = true;
		
		NewSceneSpawn(a_serialize, a_chunk, a_aiManager, spawner.Look().spawnName.at(0), kDefaultWorldPosition);
		std::vector<std::string> tmp;
		for (int i = 1; i < spawner.Look().spawnName.size(); i++)
		{
			tmp.push_back(spawner.Look().spawnName.at(i));
		}

		spawner->spawnName.swap(tmp);
	}

	if (spawn) return;
	// 既にチャンク変更待ちだった場合何もせず抜ける
	ComponentView chunkChangeView = a_chunk.GetView<ComponentTypes<DelayChunkChange>>();
	for (auto it : chunkChangeView)
	{
		return;
	}

	ComponentView uiView = a_chunk.GetView<ComponentTypes<UIComponent>>();
	for (auto it : uiView)
	{
		a_chunk.DeleteChunkEntity(it);
	}


	ResetSound();

	Entity battleClear = a_chunk.CreateNewEntity(
		CreateEffect(GAMECLEAR, float2(0.0f, 0.0f), 0.0f, 0.0f)
	);
	// a_systemResponse.AddStopTime(200.0f, 200.0f, 0.0f);

	//Entity backSound = a_chunk.CreateNewEntity(
	//	SoundKey(false, 70.0f, "bassdrum")
	//);

	Entity sound = a_chunk.CreateNewEntity(
		SoundKey(false, 70.0f, "clearbgm")
	);

	// クリアシーンに移行する
	Entity chunkChange = a_chunk.CreateNewEntity(
		ChunkChange(true, "Clear"),
		DelayChunkChange(530.0f)
	);

	Entity stopper = a_chunk.CreateNewEntity(
		AllActionStopper()
	);

	Entity movieSpawn = a_chunk.CreateNewEntity(
		SpawnJson("ClearMovie", 80.0f, kClearWorldPosition)
	);
}


void CheckClearTutorialSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{

	ComponentView enemyView = a_chunk.GetView<ComponentTypes<ClearTarget>>();
	for (auto it : enemyView)
	{
		// クリア対象が存在した場合関数を終了する
		return;
	}

	// まだ敵が残っている場合生成する
	bool spawn = false;
	ComponentView spawnerView = a_chunk.GetView<ComponentTypes<TutorialSpawner>>();
	for (auto it : spawnerView)
	{
		ComponentHandle<TutorialSpawner> spawner = a_chunk.GetComponent<TutorialSpawner>(it);
		if (spawner.Look().state.size() <= spawner.Look().progress) continue;
		spawn = true;

		const TutorialState& state = spawner.Look().state.at(spawner->progress);
		if (state.useStartMessage)
		{
			a_chunk.CreateNewEntity(
				CreateTutorialWindow(state.startMessageKey, 65.0f),
				ClearTarget(),
				AllActionStopper()
			);

		}

		if(state.useLoad) NewSceneSpawn(a_serialize, a_chunk, a_aiManager, state.load, kDefaultWorldPosition);
		spawner->progress += 1;
	}

	if (spawn) return;
	// 既にチャンク変更待ちだった場合何もせず抜ける
	ComponentView chunkChangeView = a_chunk.GetView<ComponentTypes<DelayChunkChange>>();
	for (auto it : chunkChangeView)
	{
		return;
	}

	ComponentView uiView = a_chunk.GetView<ComponentTypes<UIComponent>>();
	for (auto it : uiView)
	{
		a_chunk.DeleteChunkEntity(it);
	}


	ResetSound();

	Entity battleClear = a_chunk.CreateNewEntity(
		CreateEffect(GAMECLEAR, float2(0.0f, 0.0f), 0.0f, 0.0f)
	);
	// a_systemResponse.AddStopTime(200.0f, 200.0f, 0.0f);

	//Entity backSound = a_chunk.CreateNewEntity(
	//	SoundKey(false, 70.0f, "bassdrum")
	//);

	Entity sound = a_chunk.CreateNewEntity(
		SoundKey(false, 70.0f, "clearbgm")
	);

	// クリアシーンに移行する
	Entity chunkChange = a_chunk.CreateNewEntity(
		ChunkChange(true, "Title"),
		DelayChunkChange(530.0f)
	);

	Entity stopper = a_chunk.CreateNewEntity(
		AllActionStopper()
	);

	Entity movieSpawn = a_chunk.CreateNewEntity(
		SpawnJson("ClearMovie", 80.0f, kClearWorldPosition)
	);
}
