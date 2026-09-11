#include "ResultWorld.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"

using namespace Component;

ResultWorld::ResultWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, IEffectCacheAcquisition& a_effectCache, std::function<void(int)> a_tutorialRequest, std::function<void(std::string)> a_worldRequest, Input& a_input, ComponentsSerialize& a_serialize)
	: World(a_modelCache,
		a_uiCache,
		a_effectCache,
		a_tutorialRequest,
		a_input,
		a_serialize)
	, worldRequest(a_worldRequest)
{
	a_input.RegisterKey("ChunkChange", VK_LBUTTON);
	a_input.RegisterButton("ChunkChange", XINPUT_GAMEPAD_A);
}

Chunk ResultWorld::CreateNewChunk(AIManager& a_aiManager)
{
	(void)a_aiManager;
	Chunk newChunk;

	ResetSound();
	PlaySound(LoadSound("Assets/Sound/gameover.mp3", false));
	PlaySound(LoadSound("Assets/Sound/gameoverback.mp3", false));

	Entity gameOverLogo = newChunk.CreateNewEntity(
		UIComponent("GameOver", float2(0.0f, 0.4f), float2(1.5f, 1.2f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.01f)
	);

	Entity gameOverKeyLogo = newChunk.CreateNewEntity(
		UIComponent("PleaseKey", float2(0.0f, -0.65f), float2(1.0f, 0.3f), 0.0f),
		FadeUI(FADE_DOWN, 0.01f),
		FadeChange(FADE_CHANGE_FLICKER)
	);

	Entity gameOverCamera = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(float3(0.0f, 25.0f, -30.0f), float3(20.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		Camera(10, float3{ 0.0f, 0.0f, 1.0f }, float3{ 0.0f, 1.0f, 0.0f }, 60.0f, 1.77777, 0.05f, 1000.0f)
	);

	Entity resultChangeScene = newChunk.CreateNewEntity(
		ChunkChange(true, "Title"),
		KeyChunkChange("ChunkChange", 120.0f)
	);

	Entity resultEffectKey = newChunk.CreateNewEntity(
		EffectKey(WHITEMINIFADE_UP, "ChunkChange")
	);

	Entity createResultClear = newChunk.CreateNewEntity(
		CreateEffect(DARKFADE_CLEAR)
	);



	Entity tombstone = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 10.0f, 0.0f),
			float3(0.0f, 20.0f, 0.0f),
			float3(3.7f, 3.7f, 3.7f)
		),
		ModelKey("Box"),
		AlphaBlendComponent("Red", "Red"),
		AngularVelocity(float3(0.7f, 0.0f, 0.0f))
	);
	return newChunk;
}

void ResultWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	// Physics系の処理を行う
	WorldPowerSystem(a_chunk, a_context);
	ForceSystem(a_chunk, a_context);
	VelocitySystem(a_chunk, a_context);
	AngularVelocitySystem(a_chunk, a_context);
	PhysicsPoseSystem(a_chunk, a_context);
	ColliderSystem(a_chunk, a_context);
	ColliderCheckSystem(a_chunk, a_context);
	SectorCheckSystem(a_chunk, a_context);
	ColliderBackSystem(a_chunk, a_context);
	LatePhysicsPoseSystem(a_chunk, a_context);

	// 物理挙動の結果に基づいてキャラクターのステートを決定する
	GroundedSystem(a_chunk, a_context);
	CharacterActionMaskSystem(a_chunk, a_context);
	MoveInputResolveSystem(a_chunk, a_context, a_aiManager);

	// Pose系の処理を行う
	InputMoveSystem(a_chunk, a_context);
	InputRotatoSystem(a_chunk, a_context);
	FlipSystem(a_chunk, a_context);
	MoveForwardSystem(a_chunk, a_context);
	LeapSystem(a_chunk, a_context);
	ShakingSystem(a_chunk, a_context);
	TrackingWarpSystem(a_chunk, a_context);
	RailSystem(a_chunk, a_context);
	FlySystem(a_chunk, a_context);
	ShakeSystem(a_chunk, a_context);
	PoseSystem(a_chunk, a_context);

	// 追従などの遅延系処理
	LookMoveSystem(a_chunk, a_context);
	LookSystem(a_chunk, a_context);
	LookOnSystem(a_chunk, a_context);
	FollowTransformSystem(a_chunk, a_context);
	CameraMoveSystem(a_chunk, a_context);
	LatePoseSystem(a_chunk, a_context);
	// Transform適用
	TransformSystem(a_chunk, a_context);

	// Transform等を使用するSystemを実行
	RaySystem(a_chunk, a_context);
	CameraViewSystem(a_chunk, a_context);
	RailUpdateSystem(a_chunk, a_context);
	CameraMoveSystem(a_chunk, a_context);
	CameraViewSystem(a_chunk, a_context);
	CreateEffectSystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);

	// 終了処理
	LifeTimeSystem(a_chunk, a_context);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void ResultWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}


