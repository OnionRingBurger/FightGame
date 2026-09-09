#include "TitleWorld.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"
#include "Defines.h"

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
	a_input.RegisterButton("ChunkChange", XINPUT_GAMEPAD_A);
}

Chunk TitleWorld::CreateNewChunk(AIManager& a_aiManager)
{
	(void)a_aiManager;
	Chunk newChunk;

	ResetSound();


	Entity titleLogo = newChunk.CreateNewEntity(
		UIComponent("StartText", float2(0.0f, 0.4f), float2(1.0f, 1.0f), 0.0f)
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

	Entity titleCameraPoint = newChunk.CreateNewEntity(
		Name("CameraPoint"),
		MOVE_AND_TRANSFORM_COMPONENT(float3(-3.0f, -8.0f, -13.0f), float3(0.0f, 20.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		PosePosState(POSE_POS_LEAP),
		CameraPoint(100, 100.0f, 100.0f),
		RailFly(0.4f, 0.02f)
	);

	Entity titleCamera = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		PosePosState(POSE_POS_CAMERA),
		PoseRotState(POSE_ROT_CAMERA),
		Camera(10, float3{ 0.0f, 0.0f, 1.0f }, float3{ 0.0f, 1.0f, 0.0f }, 60.0f, 1.777777, 0.05f, 1000.0f),
		CameraMover(titleCameraPoint),
		CameraTag()
	);

	Entity changeScene = newChunk.CreateNewEntity(
		ChunkChange(true, "Proto"),
		KeyChunkChange("ChunkChange", 240.0f)
	);

	Entity character = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(-2.0f, 20.5f, -6.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		ModelKey("Box"),
		InputSource(InputOrigin::Title),
		InputMove(kPlayerMoveSpeed),
		MoveInputResult(),
		PlayerTag(),
		Firework(FIREWORK_NONE),
		GroundedState(),
		DeadState(),
		ActionMask(),
		Velocity(),
		Force(),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		OBBCollider(),
		HitInfomation(),
		JumpPower(float3(0.0f, 20.0f / 60.0f, 0.0f)),
		AttackKeyLoad({ "Player1Attack1", "Player1Attack2" }),
		LookMove(800.0f / 60.0f),
		PoseRotState(POSE_ROT_LOOKMOVE),
		HitPoint(15.0f),
		AttackHitRecord(),
		GuardState(0.2f, 3.0f),
		Name("Player"),
		InterferenceResult(),
		GhostAreaComponent(1.4f),
		LookOnState(7.0f, 7.8f),
		AlphaBlendComponent("Red", "Red")
	);

	Entity enemy1 = newChunk.CreateNewEntity(
		Name("Character"),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(6.0f, -9.5f, -0.5f),
			float3(0.0f, 42.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		ModelKey("Box"),
		AlphaBlendComponent("Red", "Purple"),
		ShakeComponent(float3(0.005f, 0.005f, 0.005f), float3(1.1f, 1.1f, 1.1f), FLT_MAX, 1.0f)
	);

	Entity enemy2 = newChunk.CreateNewEntity(
		Name("Character"),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(-4.0f, -9.5f, 2.5f),
			float3(0.0f, 42.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		ModelKey("Box"),
		AlphaBlendComponent("Blue", "Green"),
		AngularVelocity(float3(0.0f, 12.0f, 0.0f))
	);

	Entity floor = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, -50.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(kDebugWorldSize.x, 80.0f, kDebugWorldSize.z)
		),
		BoxCollider(float3(0.0f, 30.0f, 0.0f), float3(kDebugWorldSize.x, 20.0f, kDebugWorldSize.z)),
		OBBCollider(OBB_PushOutLocked),
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

	Entity worldPower = newChunk.CreateNewEntity(
		WorldPower(float3(0.0f, -1.2f / 60.0f, 0.0f), float3(0.5f, 1.0f, 0.5f))
	);

	PlaySound(LoadSound("Assets/Sound/title.mp3", true));
	return newChunk;
}

void TitleWorld::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response)
{
	// 物理挙動の結果に基づいてキャラクターのステートを決定する
	GroundedSystem(a_chunk, a_context);
	CharacterActionMaskSystem(a_chunk, a_context);

	// Pose系の処理を行う

	InputMoveSystem(a_chunk, a_context); //oo
	InputRotatoSystem(a_chunk, a_context);  //oo
	FlipSystem(a_chunk, a_context); //oo
	LookSystem(a_chunk, a_context); //oo
	MoveForwardSystem(a_chunk, a_context); //oo
	LeapSystem(a_chunk, a_context);//oo
	ShakingSystem(a_chunk, a_context); //oo
	TrackingWarpSystem(a_chunk, a_context);//oo
	RailSystem(a_chunk, a_context);
	FlySystem(a_chunk, a_context);
	ShakeSystem(a_chunk, a_context);
	PoseSystem(a_chunk, a_context);

	ResetConsoleSystem(a_chunk, a_context);

	// 追従などの遅延系処理
	LookMoveSystem(a_chunk, a_context);
	FollowTransformSystem(a_chunk, a_context);//o
	CameraMoveSystem(a_chunk, a_context);//oo
	LatePoseSystem(a_chunk, a_context);
	// Transform適用
	TransformSystem(a_chunk, a_context);

	EnemyAttackLoadSystem(a_chunk, a_context);

	// リザルトなどをリセット
	ResetSystem(a_chunk, a_context);
}



void TitleWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
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

	// キャラクターのシステム
	AttackHitSystem(a_chunk, a_context, a_response, a_aiManager);
	CharacterAttackSystem(a_chunk, a_context);
	PlayerJumpSystem(a_chunk, a_context);
	AttackStartupSystem(a_chunk, a_context);
	AttackActionSystem(a_chunk, a_context);
	AttackWaitActionSystem(a_chunk, a_context);
	JumpPhysicsSystem(a_chunk, a_context);
	GuardSystem(a_chunk, a_context);
	GuardActionSystem(a_chunk, a_context);
	HitPointSystem(a_chunk, a_context);
	AttackHitRecordCleanupSystem(a_chunk, a_context);
	AttackInstanceEndCheckSystem(a_chunk, a_context);
	AttackInstanceResolveSystem(a_chunk, a_context, a_aiManager);
	EntryActionSystem(a_chunk, a_context);
	PlayerDeadSystem(a_chunk, a_context, a_response);
	EnemyDeadSystem(a_chunk, a_context, a_response);
	BossDeadSystem(a_chunk, a_context, a_response);

	CameraViewSystem(a_chunk, a_context);
	CreateEffectSystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);

	// 終了処理
	LifeTimeSystem(a_chunk, a_context);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void TitleWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}


