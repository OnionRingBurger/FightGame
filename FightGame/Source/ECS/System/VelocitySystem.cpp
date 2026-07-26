#include "VelocitySystem.h"
#include "Components.h"

using namespace Component;

void MoveForwardSystem(Chunk& a_chunk, const SystemContext& a_context)
{

	ComponentView view = a_chunk.GetView<ComponentTypes<MoveForward, MotionResult, Rotation>>();

	for (auto it : view)
	{
		ComponentHandle<MotionResult> result = a_chunk.GetComponent<MotionResult>(it);
		const ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(it);
		ComponentHandle<MoveForward> move = a_chunk.GetComponent<MoveForward>(it);

		move->speed *= pow(move.Look().attenuation, a_context.deltaTime);

		float3 radRotation(rot.Look().pitch * RAD, rot.Look().yaw * RAD, rot.Look().roll * RAD);

		float3 forward;
		forward.x = std::sin(radRotation.y) * std::cos(radRotation.x); 
		forward.y = std::sin(-radRotation.x);                  
		forward.z = std::cos(radRotation.y) * std::cos(radRotation.x);

		float3 displacement = forward * move.Look().speed * a_context.deltaTime;

		result->posOffset.x += displacement.x;
		result->posOffset.y += displacement.y;
		result->posOffset.z += displacement.z;
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
	float3 totalPower(0.0f, 0.0f, 0.0f);
	bool hasPower = false;

	for (auto it : powerView)
	{
		const ComponentHandle<WorldPower> worldPower = a_chunk.GetComponent<WorldPower>(it);
		totalPower.x += worldPower.Look().power.x;
		totalPower.y += worldPower.Look().power.y;
		totalPower.z += worldPower.Look().power.z;
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
	}
}
