#include "LoadWorld.h"
#include "Components.h"
#include "System.h"
#include "Defines.h"

using namespace Component;

LoadWorld::LoadWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, IEffectCacheAcquisition& a_effectCache, Input& a_input, ComponentsSerialize& a_serialize) : World(a_modelCache, a_uiCache, a_effectCache, nullptr, a_input, a_serialize)
{
}

Chunk LoadWorld::CreateNewChunk(AIManager& a_aiManager)
{
	(void)a_aiManager;
	Chunk newChunk;

	RegisterAllAttackSectorsFromAttackData(kSectorVertexCount);

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
		UIComponent("LoadSprite", float2(0.9f, -0.9f), float2(0.1f, 0.1f * ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)), 0.0f, 0.8f, float2(0.5f, 0.0f), float2(0.3f, 1.0f)),
		SpriteAnimation(90, int2(10, 9), true, 1.0f)
	);


	return newChunk;
}

void LoadWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	UIMoveSystem(a_chunk, a_context);
	UVMoveSystem(a_chunk, a_context);

	SpriteAnimationSystem(a_chunk, a_context);
}



