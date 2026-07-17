#include "CreateGameWorld.h"
#include "Components.h"
#include "json.hpp"
#include "ComponentsSerialize.h"
#include "System.h"

constexpr const char* startWorld = "testEntities";

CreateGameWorld::CreateGameWorld(IModelCacheAcquisition& a_modelCache,
	IUICacheAcquisition& a_uiCache,
	std::function<void(int)> a_tutorialRequest,
	Input& a_input) :
	World(a_modelCache,
		a_uiCache,
		a_tutorialRequest,
		a_input
	)
{
}

Chunk CreateGameWorld::CreateNewChunk()
{
	Chunk newChunk;

	std::unique_ptr<ComponentsSerialize> serialize = std::make_unique<ComponentsSerialize>();
	TestRegisterComponent(*serialize);

	LoadJsonComponent(newChunk, *serialize, startWorld);
	
	Entity debugCamera = newChunk.CreateNewEntity(
		DebugCameraTag(),
		Camera(),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_DEBUGCAMERA),
		PoseRotState(POSE_ROT_DEBUGCAMERA)
	);

	Entity debugCameraPoint = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		InputMove(),
		InputRotato(),
		CameraPoint(1000000000, 10000.0f, 10000.f)
	);

	return newChunk;
}

void CreateGameWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse)
{
	PoseSystem(a_chunk, a_context);
	DebugCameraSystem(a_chunk);
	LatePoseSystem(a_chunk, a_context);
	TransformSystem(a_chunk, a_context);
}

void CreateGameWorld::UpdateIMGUI(Chunk& a_chunk, SystemContext& a_context)
{
	
}

void CreateGameWorld::HandleSystemResponse(SystemResponse& a_response)
{
	
}


