#include "GameLoopSystem.h"
#include "Components.h"
#include "Sound.h"
#include "SystemAssist.h"

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



void ChunkChangeSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_chunkRequest)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<ChunkChange>>();

	for (auto it : view)
	{
		ComponentHandle<ChunkChange> chunkChange = a_chunk.GetComponent<ChunkChange>(it);
		const ComponentHandle<KeyChunkChange> keyChunkChange = a_chunk.GetComponent<KeyChunkChange>(it);
		ComponentHandle<DelayChunkChange> delayChunkChange = a_chunk.GetComponent<DelayChunkChange>(it);


		if (keyChunkChange.IsValid() && a_context.input.IsRegisterTrigger(keyChunkChange.Look().key))
		{
			if (!delayChunkChange.IsValid()) a_chunk.AddComponent(it, DelayChunkChange(keyChunkChange.Look().waitTime));
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
}

void CheckAliveTargetEnemySystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	ComponentView enemyView = a_chunk.GetView<ComponentTypes<EnemyTag, ClearTarget>>();
	for(auto it : enemyView)
	{
		// クリア対象が存在した場合関数を終了する
		return;
	}

	bool spawn = false;
	ComponentView spawnerView = a_chunk.GetView<ComponentTypes<PhaseSpawner>>();
	for (auto it : spawnerView)
	{
		ComponentHandle<PhaseSpawner> spawner = a_chunk.GetComponent<PhaseSpawner>(it);
		if (spawner.Look().spawnName.empty()) continue;
		spawn = true;

		NewEnemySpawn(a_serialize, a_chunk, a_aiManager, spawner.Look().spawnName.at(0));
		std::vector<std::string> tmp;
		for (int i = 1; i < spawner.Look().spawnName.size(); i++)
		{
			tmp.push_back(spawner.Look().spawnName.at(i));
		}

		spawner->spawnName.swap(tmp);
	}

	if (spawn) return;

	ComponentView chunkChangeView = a_chunk.GetView<ComponentTypes<DelayChunkChange>>();

	for (auto it : chunkChangeView)
	{
		return;
	}

	PlaySound(LoadSound("Shot"));
	a_systemResponse.AddStopTime(200.0f, 200.0f, 0.3f);

	// クリアシーンに移行する
	Entity chunkChange = a_chunk.CreateNewEntity(
		ChunkChange(true, "Clear"),
		DelayChunkChange(60.0f)
	);

}
