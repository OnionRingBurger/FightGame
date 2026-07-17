#include "CharacterSystem.h"

using namespace Component;

void CharacterKill(Chunk& a_chunk, Entity killEntity);


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