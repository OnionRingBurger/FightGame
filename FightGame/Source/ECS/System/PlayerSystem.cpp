#include "PlayerSystem.h"
#include "Components.h"
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


