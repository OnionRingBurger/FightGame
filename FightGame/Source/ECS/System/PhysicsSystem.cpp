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

void ForceSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<Velocity, Force>>();

	for (auto it : view)
	{
		ComponentHandle<Velocity> velocity = a_chunk.GetComponent<Velocity>(it);
		ComponentHandle<Force> force = a_chunk.GetComponent<Force>(it);
		force->force.x *= pow(force.Look().attenuation, a_context.deltaTime);
		force->force.y *= pow(force.Look().attenuation, a_context.deltaTime);
		force->force.z *= pow(force.Look().attenuation, a_context.deltaTime);
		velocity->x += force.Look().force.x * a_context.deltaTime;
		velocity->y += force.Look().force.y * a_context.deltaTime;
		velocity->z += force.Look().force.z * a_context.deltaTime;

	}
}

void WorldPowerSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView powerView = a_chunk.GetView<ComponentTypes<WorldPower>>();
	float3 totalPower;
	float3 totalRate(1.0f, 1.0f, 1.0f);
	bool hasPower = false;

	for (auto it : powerView)
	{
		const ComponentHandle<WorldPower> worldPower = a_chunk.GetComponent<WorldPower>(it);
		totalPower += worldPower.Look().power;


		totalRate.x *= worldPower.Look().rate.x;
		totalRate.y *= worldPower.Look().rate.y;
		totalRate.z *= worldPower.Look().rate.z;
		hasPower = true;
	}

	if (!hasPower) return;

	ComponentView velocityView = a_chunk.GetView<ComponentTypes<Velocity>>();
	for (auto it : velocityView)
	{
		ComponentHandle<Velocity> velocity = a_chunk.GetComponent<Velocity>(it);
		velocity->x += totalPower.x * a_context.deltaTime;
		velocity->y += totalPower.y * a_context.deltaTime;
		velocity->z += totalPower.z * a_context.deltaTime;

		velocity->x *= totalRate.x;
		velocity->y *= totalRate.y;
		velocity->z *= totalRate.z;
	}
}