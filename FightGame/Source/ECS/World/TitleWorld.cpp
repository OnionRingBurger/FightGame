#include "TitleWorld.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"

using namespace Component;

TitleWorld::TitleWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, std::function<void(int)> a_tutorialRequest, std::function<void(std::string)> a_worldRequest, Input& a_input)
	: World(a_modelCache,
		a_uiCache,
		a_tutorialRequest,
		a_input)
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
	PlaySound(LoadSound("Assets/Sound/title.mp3", true));

	Entity titleLogo = newChunk.CreateNewEntity(
		UIComponent("TitleLogo", float2(0.0f, 0.4f), float2(1.0f, 1.0f), 0.0f)
	);

	Entity keyLogo = newChunk.CreateNewEntity(
		UIComponent("PleaseKey", float2(0.0f, -0.5f), float2(1.0f, 0.3f), 0.0f),
		FadeUI(FADE_DOWN, 0.01f),
		FadeChange(FADE_CHANGE_FLICKER)
	);

	Entity titleMask = newChunk.CreateNewEntity(
		UIComponent("DarkMask", float2(0.0f, 0.0f), float2(2.0f, 2.0f), 0.0f)
	);

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
		MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 10.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		PosePosState(POSE_POS_LEAP),
		LeapPosComponent(true, float3(0.0f, 13.5f, 0.0f), float3(0.0f, 10.5f, 20.0f), 300.0f),
		LeapRotComponent(true, float3(20.0f, 0.0f, 0.0f), float3(5.0f, 0.0f, 0.0f), 300.0f),
		Velocity(0.0f, 0.0f, 0.05f),
		CameraPoint(100.0f, 100.0f, 100.0f)
	);

	Entity changeScene = newChunk.CreateNewEntity(
		ChunkChange(true, "Proto"),
		KeyChunkChange("ChunkChange", 240.0f)
	);

	Entity forwardDark = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(kTonnelWidth, kTonnelHeight, 0.1f)
		),
		PosePosState(POSE_POS_FOLLOW),
		FollowPosition(0.0f, 0.0f, kTitleTonnelSegmentLength * (kTitleTonnelSegmentCount - 2), titleCameraPoint, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y),
		ModelKey("Dark", float3(), float3(), MODEL_DRAW_SHADOW)
	);

	for (int i = 0; i < kTitleTonnelSegmentCount; i++)
	{
		Entity titleTonnel = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, kTitleTonnelHeight / 2, kTitleTonnelSegmentLength * i), float3(0.0f, 0.0f, 0.0f), float3(0.1f, 0.1f, 0.1f)),
			TrackingWarp(
				titleCamera,
				float3(0.0f, 0.0f, -kTitleTonnelSegmentLength),
				float3(0.0f, 0.0f, kTitleTonnelSegmentLength * (kTitleTonnelSegmentCount - 1) - kTitleTonnelWarpMargin),
				TRACK_FIXED_X | TRACK_FIXED_Y)
		);

		Entity titleFloor1 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(kTitleTonnelMeshSize, 1.0f, kTitleTonnelFloorDepth)),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(0.0f, -kTitleTonnelHeight / 2, 0.0f, titleTonnel)
		);

		Entity titleFloor2 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(kTitleTonnelMeshSize, 1.0f, kTitleTonnelFloorDepth)),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(0.0f, kTitleTonnelHeight / 2, 0.0f, titleTonnel)
		);

		Entity titleFloor3 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, kTitleTonnelMeshSize, kTitleTonnelFloorDepth)),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(kTitleTonnelWidth / 2, 0.0f, 0.0f, titleTonnel)
		);

		Entity titleFloor4 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, kTitleTonnelMeshSize, kTitleTonnelFloorDepth)),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(-kTitleTonnelWidth / 2, 0.0f, 0.0f, titleTonnel)
		);

		Entity titleFloor5 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 45.0f), float3(1.0f, kTitleTonnelMeshSize, kTitleTonnelFloorDepth)),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(kTitleTonnelWidth / 3.0f, kTitleTonnelHeight / 3.0f, 0.0f, titleTonnel)
		);

		Entity titleFloor6 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 225.0f), float3(kTitleTonnelMeshSize, 1.0f, kTitleTonnelFloorDepth)),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(-kTitleTonnelWidth / 3.0f, kTitleTonnelHeight / 3.0f, 0.0f, titleTonnel)
		);
	}

	return newChunk;
}

void TitleWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager)
{
	VelocitySystem(a_chunk, a_context);
	LeapSystem(a_chunk, a_context);
	TrackingWarpSystem(a_chunk, a_context);
	PoseSystem(a_chunk, a_context);

	FollowTransformSystem(a_chunk, a_context);
	CameraMoveSystem(a_chunk, a_context);
	LatePoseSystem(a_chunk, a_context);
	TransformSystem(a_chunk, a_context);

	CameraViewSystem(a_chunk, a_context);
	CreateEffectSystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);

	// I—¹ˆ—
	LifeTimeSystem(a_chunk, a_context);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void TitleWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}


