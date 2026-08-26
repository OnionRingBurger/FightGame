#include "LoadWorld.h"
#include "Components.h"
#include "System.h"

using namespace Component;

LoadWorld::LoadWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, IEffectCacheAcquisition& a_effectCache, Input& a_input, ComponentsSerialize& a_serialize) : World(a_modelCache, a_uiCache, a_effectCache, nullptr, a_input, a_serialize)
{
}

Chunk LoadWorld::CreateNewChunk(AIManager& a_aiManager)
{
	(void)a_aiManager;
	Chunk newChunk;

	newChunk.CreateNewEntity(
		CameraTag(),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_CAMERA),
		PoseRotState(POSE_ROT_CAMERA),
		Camera(1, float3(0.0f, 0.0f, 20.0f), float3(0.0f, 1.0f, 0.0f), 60.0f, 16.0f / 9.0f, 0.05f, 1000.0f)
	);

	newChunk.CreateNewEntity(
		UIComponent("LoadWind", float2(0.8f, -0.8f), float2(0.32f, 0.3f), 0.0f, 0.8f, float2(0.5f, 0.0f), float2(0.3f, 1.0f)),
		UVMove(float2(0.01f, 0.0f))
	);

	newChunk.CreateNewEntity(
		UIComponent("LoadRail", float2(0.795f, -0.8f), float2(0.28f, 0.35f), 0.0f),
		UVMove(float2(0.025f, 0.0f))
	);

	newChunk.CreateNewEntity(
		UIComponent("LoadRail", float2(0.795f, -0.8f), float2(0.28, 0.35f), 0.0f, 1.0f, float2(0.5f, 0.0f), float2(1.0f, 1.0f)),
		UVMove(float2(0.025f, 0.0f))
	);

	newChunk.CreateNewEntity(
		UIComponent("LoadMainCar", float2(0.8f, -0.8f), float2(0.28f, 0.35f), 0.0f)
	);

	newChunk.CreateNewEntity(
		UIComponent("LoadWind", float2(0.8f, -0.8f), float2(0.32f, 0.3f), 0.0f, 0.8f, float2(0.0f,0.0f), float2(0.3f, 1.0f)),
		UVMove(float2(0.015f, 0.0f))
	);
	return newChunk;
}

void LoadWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	UIMoveSystem(a_chunk, a_context);
	UVMoveSystem(a_chunk, a_context);
}



