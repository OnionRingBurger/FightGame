#include "TransformSystem.h"
#include "Components.h"

using namespace Component;

void VelocitySystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<Velocity, MotionResult>>();

	for (auto it : view)
	{
		// Positionを動かすためVelocityは変更しない形で取得
		const ComponentHandle<Velocity> velocity = a_chunk.GetComponent<Velocity>(it);
		ComponentHandle<MotionResult> result = a_chunk.GetComponent<MotionResult>(it);

		result->posOffset.x += velocity.Look().x * a_context.deltaTime;
		result->posOffset.y += velocity.Look().y * a_context.deltaTime;
		result->posOffset.z += velocity.Look().z * a_context.deltaTime;

	}
}


void AngularVelocitySystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<AngularVelocity , MotionResult>>();
	for (auto it : view)
	{
		// Positionを動かすためVelocityは変更しない形で取得
		const ComponentHandle<AngularVelocity> velocity = a_chunk.GetComponent<AngularVelocity>(it);
		ComponentHandle<MotionResult> result = a_chunk.GetComponent<MotionResult>(it);

		result->rotOffset += velocity.Look().velocity * a_context.deltaTime;
	}
}

