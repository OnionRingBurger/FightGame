#include "ClearWorld.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"

using namespace Component;

ClearWorld::ClearWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, std::function<void(int)> a_tutorialRequest, std::function<void(std::string)> a_worldRequest, Input& a_input)
	: World(a_modelCache,
		a_uiCache,
		a_tutorialRequest,
		a_input)
	, worldRequest(a_worldRequest)
{
	a_input.RegisterKey("ChunkChange", VK_LBUTTON);
	a_input.RegisterButton("ChunkChange", VK_PAD_A);
}

Chunk ClearWorld::CreateNewChunk(AIManager& a_aiManager)
{
	(void)a_aiManager;
	Chunk newChunk;

	ResetSound();
	PlaySound(LoadSound("Assets/Sound/clear.mp3", true));
	PlaySound(LoadSound("Assets/Sound/clearWind.mp3", true));

	Entity clearChangeScene = newChunk.CreateNewEntity(
		ChunkChange(true, "Title"),
		KeyChunkChange("ChunkChange", 120.0f)
	);

	Entity clearEffectKey = newChunk.CreateNewEntity(
		EffectKey(WHITEMINIFADE_UP, "ChunkChange")
	);

	Entity gameClearLogo = newChunk.CreateNewEntity(
		UIComponent("GameClear", float2(0.0f, 0.4f), float2(0.7f, 0.7f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.01f)
	);

	Entity gameClearText = newChunk.CreateNewEntity(
		UIComponent("GameClearText", float2(0.0f, -0.3f), float2(1.4f, 0.3f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.01f)
	);

	// TODO Leapがなくなった後Cameraに移行しないため後で直す
	Entity clearCameraPoint = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 190.0f, 100.0f), float3(-9.2f, -3.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		PosePosState(POSE_POS_LEAP),
		LeapPosComponent(true, float3(0.0f, 900.0, 100.0f), float3(0.0f, 71.0, 100.0f), 360.0f),
		CameraPoint(100, 100.0f, 0.0f),
		LifeTime(360.0f)
	);

	Entity clearCameraPoint2 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 70.0f, 100.0f), float3(-9.2f, -3.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		ShakingComponent(false, 35.0f, 0.0025f, 1.0f, SHAKING_POSITION),
		CameraPoint(50, 0.04f, 0.0f)
	);

	Entity clearCamera = newChunk.CreateNewEntity(
		CameraTag{},
		Camera(100, float3{ 0.0f, 0.0f, 0.0f }, float3{ 0.0f, 1.0f, 0.0f }, 60.0f, 16.0f / 9.0f, 0.05f, 300.0f),
		MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 70.0f, 100.0f), float3(-9.2f, -3.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		PosePosState(POSE_POS_CAMERA),
		PoseRotState(POSE_ROT_CAMERA)
	);

	Entity mapModel = newChunk.CreateNewEntity(
		ModelKey("low_poly_treesNXT_5flat", float3(), float3(), MODEL_DRAW_SHADOW),
		MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(-90.0f, 0.0f, 0.0f), float3(0.05f, 0.05f, 0.05f))
	);

	return newChunk;
}

void ClearWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager)
{
	
	VelocitySystem(a_chunk, a_context);
	LeapSystem(a_chunk, a_context);
	ShakingSystem(a_chunk, a_context);
	FollowTransformSystem(a_chunk, a_context);
	CameraMoveSystem(a_chunk, a_context);
	// HitBoxやPosition等を使用するSystemを実行
	CameraViewSystem(a_chunk, a_context);
	CreateEffectSystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);

	// 終了処理
	LifeTimeSystem(a_chunk, a_context);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void ClearWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}




