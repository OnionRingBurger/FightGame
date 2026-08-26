#include "TitleWorld.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"

using namespace Component;

TitleWorld::TitleWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, IEffectCacheAcquisition& a_effectCache, std::function<void(int)> a_tutorialRequest, std::function<void(std::string)> a_worldRequest, Input& a_input, ComponentsSerialize& a_serialize)
	: World(a_modelCache,
		a_uiCache,
		a_effectCache,
		a_tutorialRequest,
		a_input,
		a_serialize)
	, worldRequest(a_worldRequest)
{
	a_input.RegisterKey("ChunkChange", VK_LBUTTON);
	a_input.RegisterButton("ChunkChange", VK_PAD_A);
}

Chunk TitleWorld::CreateNewChunk(AIManager& a_aiManager)
{
	(void)a_aiManager;
	Chunk newChunk;

	ResetSound();


	Entity titleLogo = newChunk.CreateNewEntity(
		UIComponent("TitleLogo", float2(0.0f, 0.4f), float2(1.0f, 1.0f), 0.0f)
	);

	Entity keyLogo = newChunk.CreateNewEntity(
		UIComponent("PleaseKey", float2(0.0f, -0.5f), float2(1.0f, 0.3f), 0.0f),
		FadeUI(FADE_DOWN, 0.01f),
		FadeChange(FADE_CHANGE_FLICKER)
	);

	//Entity titleMask = newChunk.CreateNewEntity(
	//	UIComponent("DarkMask", float2(0.0f, 0.0f), float2(2.0f, 2.0f), 0.0f)
	//);

	Entity createEffectKey = newChunk.CreateNewEntity(
		EffectKey(WHITEFADE_UP, "ChunkChange")
	);

	Entity createEffect = newChunk.CreateNewEntity(
		CreateEffect(WHITEMINIFADE_CLEAR)
	);

	Entity titleCamera = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		PosePosState(POSE_POS_CAMERA),
		PoseRotState(POSE_ROT_CAMERA),
		Camera(10, float3{ 0.0f, 0.0f, 1.0f }, float3{ 0.0f, 1.0f, 0.0f }, 60.0f, 16 / 9, 0.05f, 1000.0f)
	);

	Entity titleCameraPoint = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(float3(-3.0f, 0.0f, -15.0f), float3(32.0f, 20.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		PosePosState(POSE_POS_LEAP),
		/*LeapRotComponent(true, float3(60.0f, 0.0f, 0.0f), float3(5.0f, 0.0f, 0.0f), 300.0f),*/
		// Velocity(0.0f, 0.0f, 0.05f),
		CameraPoint(100.0f, 100.0f, 100.0f),
		RailFly(1.0f, 0.02f)
	);

	Entity changeScene = newChunk.CreateNewEntity(
		ChunkChange(true, "Proto"),
		KeyChunkChange("ChunkChange", 240.0f)
	);

	Entity character = newChunk.CreateNewEntity(
		ModelKey("Box")
	);

	Entity floor = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, -50.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(kDebugWorldSize.x, 80.0f, kDebugWorldSize.z)
		),
		ModelKey("Box"),
		AlphaBlendComponent("Red", "Blue")
	);

	Entity underFloor = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, -40.0f, 10.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(kDebugWorldSize.x * 10.0f, 1.0, kDebugWorldSize.z * 6.0f)
		),
		ModelKey("Box"),
		AlphaBlendComponent("White", "White")
	);
	
	Entity sky = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 80.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(kDebugWorldSize.x * 10.0f, 180.0, 1.0f)
		),
		ModelKey("Box"),
		AlphaBlendComponent("White", "Blue")
	);

	PlaySound(LoadSound("Assets/Sound/title.mp3", true));
	return newChunk;
}

void TitleWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	VelocitySystem(a_chunk, a_context);
	LeapSystem(a_chunk, a_context);
	TrackingWarpSystem(a_chunk, a_context);
	FlySystem(a_chunk, a_context);
	PoseSystem(a_chunk, a_context);

	FollowTransformSystem(a_chunk, a_context);
	CameraMoveSystem(a_chunk, a_context);
	LatePoseSystem(a_chunk, a_context);
	TransformSystem(a_chunk, a_context);

	CameraViewSystem(a_chunk, a_context);
	CreateEffectSystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);

	// èIóπèàóù
	LifeTimeSystem(a_chunk, a_context);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void TitleWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}


