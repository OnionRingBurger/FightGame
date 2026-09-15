#include <fstream>
#include "CreateGameWorld.h"
#include "Components.h"
#include "json.hpp"
#include "ComponentsSerialize.h"
#include "System.h"
#include "DebugSystemResponse.h"

#include "Defines.h"
constexpr const char* startWorld = "GameControllerUI";

CreateGameWorld::CreateGameWorld(IModelCacheAcquisition& a_modelCache,
	IUICacheAcquisition& a_uiCache,
	IEffectCacheAcquisition& a_effectCache,
	std::function<void(int)> a_tutorialRequest,
	Input& a_input,
	ComponentsSerialize& a_serialize) :
	World(a_modelCache,
		a_uiCache,
		a_effectCache,
		a_tutorialRequest,
		a_input,
		a_serialize
	),
	name(startWorld),
	isSave(false),
	isLoad(false)
{	
}


Chunk CreateGameWorld::CreateNewChunk(AIManager& a_aiManager)
{
	(void)a_aiManager;
	Chunk newChunk;

	std::unique_ptr<ComponentsSerialize> serialize = std::make_unique<ComponentsSerialize>();
	TestRegisterComponentType(*serialize);

	LoadJsonComponent(newChunk, *serialize, startWorld);
	

	CreateDebugComponent(newChunk);

	//newChunk.CreateNewEntity(
	//	Component::SpawnJson("GameControllerUI", 0.0f, float3())
	//);

	return newChunk;
}

void CreateGameWorld::InitResponse(std::unique_ptr<SystemResponse>& response)
{
	response = std::make_unique<DebugSystemResponse>(name);
}

void CreateGameWorld::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	SpawnJsonSystem(a_chunk, a_context, a_serialize, a_aiManager);
}

void CreateGameWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	// !!!New!!!
	if (isLoad)
	{
		isLoad = false;
		LoadJsonComponent(a_chunk, a_serialize, name);
		CreateDebugComponent(a_chunk);
		return;
	}

	if (isSave)
	{
		std::ifstream ifstream = std::ifstream(kDataPath);
		if(!ifstream.is_open())
		{
			isSave = false;
			CreateDebugComponent(a_chunk);
			return;
		}

		nlohmann::ordered_json json;
		json << ifstream;
		json[name] = OutJsonComponents(a_chunk, a_serialize);
		std::ofstream of(kDataPath);
		if (of.is_open())
		{
			of << json.dump(4);
		}
		isSave = false;
		CreateDebugComponent(a_chunk);
		return;
	}

	PoseSystem(a_chunk, a_context);
	DebugCameraSystem(a_chunk);
	LatePoseSystem(a_chunk, a_context);
	TransformSystem(a_chunk, a_context);

	ImGuiSystem(a_chunk, a_context);
	SaveWorldSystem(a_chunk, a_context, a_systemResponse);
}

void CreateGameWorld::UpdateIMGUI(Chunk& a_chunk, SystemContext& a_context)
{
	ImGuiSystem(a_chunk, a_context);
}

void CreateGameWorld::HandleSystemResponse(SystemResponse& a_response)
{
	DebugSystemResponse& response = dynamic_cast<DebugSystemResponse&>(a_response);
	isSave = false;
	isLoad = false;

	if (response.IsSave())
	{
		isSave = true;
	}

	// !!!New!!!
	if (response.IsLoad())
	{
		isLoad = true;
		name = response.GetKey();
	}
}

void CreateGameWorld::CreateDebugComponent(Chunk& a_chunk)
{
	Entity debugCamera = a_chunk.CreateNewEntity(
		DebugCameraTag(),
		DebugEntityTag(),
		Camera(10, float3(0.0f, 0.0f, 1.0f), float3(0.0f, 1.0f, 0.0f), 60.0f, 1.77777f, 0.05f, 1000.0f),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		Name("DebugCamera"),
		PosePosState(POSE_POS_DEBUGCAMERA),
		PoseRotState(POSE_ROT_DEBUGCAMERA)
	);

	Entity debugCameraPoint = a_chunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 20.0f, 0.0f),
			float3(0.0f, 90.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		Name("DebugCameraPoint"),
		DebugEntityTag(),
		InputMove(),
		InputRotato(),
		CameraPoint(1000000000, 10000.0f, 10000.f)
	);

	Entity debugFloor = a_chunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, -40.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(kDebugWorldSize.x, 80.0f, kDebugWorldSize.z)
		),
		Name("DebugCameraPoint"),
		DebugEntityTag(),
		ModelKey("Box"),
		AlphaBlendComponent("Red", "Blue")
	);

	//Entity cameraRig = a_chunk.CreateNewEntity(
	//	MOVE_AND_TRANSFORM_COMPONENT(
	//		float3(0.0f, 0.0f, 0.0f),
	//		float3(55.0f, 0.0f, 0.0f),
	//		float3(1.0f, 1.0f, 1.0f)
	//	),
	//	LeapPosComponent(true, float3(0.0f, 0.7f, -5.0f) + kDefaultWorldPosition, float3(0.0f, 0.0f, 3.0f) + kDefaultWorldPosition, 65.0f),
	//	PosePosState(POSE_POS_FOLLOW),
	//	CameraPoint(200.0f, 100.0f, 100.0f, float3(0.0f, 0.0f, -15.0f)),
	//	LifeTime(120.0f)
	//);

}


