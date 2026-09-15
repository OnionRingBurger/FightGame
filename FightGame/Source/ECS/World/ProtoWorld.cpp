#include "ProtoWorld.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"
#include "json.hpp"
#include "DebugConsole.h"
#include "ComponentsSerialize.h"
#include "AI.h"
#include "Defines.h"
#include "Geometory.h"
#include "SystemAssist.h"
#include "GameSystemResponse.h"

#include <fstream>
#include <vector>

ProtoWorld::ProtoWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, IEffectCacheAcquisition& a_effectCache, std::function<void(int)> a_tutorialRequest, std::function<void(std::string)> a_worldRequest, std::function<void(int)> a_stageRequest, Input& a_input, ComponentsSerialize& a_serialize, int a_stage)
	: World(a_modelCache,
		a_uiCache,
		a_effectCache,
		a_tutorialRequest,
		a_input,
		a_serialize
	)
	, worldRequest(a_worldRequest)
	, stageRequest(a_stageRequest)
	, stage(a_stage)
{
	AddGameBind(a_input);
}

void ProtoWorld::InitAI(AIManager& a_aiManager)
{
	std::unique_ptr<BehaviorTree> bt = std::make_unique<BehaviorTree>();
	RootNode& root = bt->GetRoot();

	std::unique_ptr<ReactiveSelector> fightSelector = std::make_unique<ReactiveSelector>();

	fightSelector->AddNode(std::make_unique<InAttackRangeDecorator>(std::make_unique<UseAttackAction>()));
	fightSelector->AddNode(std::make_unique<ChasePlayerAction>(1.0f));

	std::unique_ptr<ReactiveSelector> neutralSelector = std::make_unique<ReactiveSelector>();
	neutralSelector->AddNode(std::make_unique<InAttackRangeDecorator>(std::make_unique<UseAttackAction>()));
	neutralSelector->AddNode(std::make_unique<ChasePlayerAction>(1.0f));
	

	std::unique_ptr<ReactiveSelector> guardSelector = std::make_unique<ReactiveSelector>();
	guardSelector->AddNode(std::make_unique<UseBestDamageAttackAction>());
	guardSelector->AddNode(std::make_unique<GuardNode>());


	std::unique_ptr<StanceNode> stance = std::make_unique<StanceNode>(
		std::move(guardSelector),
		std::move(fightSelector),
		std::move(neutralSelector)
	);

	root.AddNode(std::move(stance));

	a_aiManager.RegisterTree("Enemy", std::move(bt));
}

Chunk ProtoWorld::CreateNewChunk(AIManager& a_aiManager)
{
	
	Chunk newChunk;

	Entity player = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 65.5f, -5.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		ModelKey("Box"),
		InputSource(InputOrigin::Device),
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
		AlphaBlendComponent("Red", "Red"),
		EntryAction(120.0f),
		GameOverTarget()
	);

	Entity targetMarker = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		LookOnMarkerTag(),
		Name("TargetMarker"),
		FollowPosition(float3(), kInvalidEntity),
		PosePosState(POSE_POS_FOLLOW),
		UIComponent("LookOnMaker", float2(0.0f, 0.0f), float2(0.6f, 0.6f), 0.0f),
		SpriteComponent(float3(0.0f, 2.5f, 0.0f), float3(), true),
		RailFly(0.08f, 0.09f)
	);

	// !!!New!!!
	std::vector<std::string> phaseKeys;

	StageData data;

	AlphaBlendComponent floorColor;
	AlphaBlendComponent seaColor;

	switch (stage)
	{
	case 2:
		phaseKeys =
		{
			"Stage2Phase1",
			"Stage2Phase2",
			"Stage2Phase3",
			"PhaseClear",
			"Stage2Boss1"
		};

		data = StageData(stage, false, 0);

		PlaySound(LoadSound("Assets/Sound/gamebgm2.mp3", true));
		floorColor = AlphaBlendComponent("Red", "Blue");
		seaColor = AlphaBlendComponent("Purple", "WaveFadeMask_A");

		break;
	default:
		phaseKeys =
		{
			"Stage1Phase1",
			"Stage1Phase2",
			"PhaseClear",
			"Stage1Phase3",
			"Stage1Phase4",
			"PhaseClear",
			"Stage1Boss1"
		};

		PlaySound(LoadSound("Assets/Sound/gamebgm.mp3", true));
		floorColor = AlphaBlendComponent("Red", "Blue");
		seaColor = AlphaBlendComponent("White", "Blue");
		data = StageData(stage, true, 2);

		break;
	}

	Entity spawner = newChunk.CreateNewEntity(
		PhaseSpawner(phaseKeys)
	);

	Entity stageData = newChunk.CreateNewEntity(
		Name("StageData"),
		data
	);

	Entity hpBack = newChunk.CreateNewEntity(
		UIComponent("UIBack", float2(-0.58f, 0.9f), float2(0.791f, 0.08f), 0.0f)
	);


	Entity hpGauge = newChunk.CreateNewEntity(
		UIComponent("UIGauge", float2(-0.58f, 0.9f), float2(0.791f, 0.08f), 0.0f),
		HPGaugeUI(player, float2(-0.58f, 0.9f), float2(0.791f, 0.08f))

	);



		//Entity ButtonUIA = newChunk.CreateNewEntity(
		//	UIComponent("ButtonLB", float2(-0.93f, -0.88f), float2(0.15f * 0.8f, 0.275f * 0.8f), 0.0f)
		//);

		//Entity GuardUI = newChunk.CreateNewEntity(
		//	UIComponent("GuardUI", float2(-0.75f, -0.88f), float2(0.28f * 0.8f, 0.22f * 0.8f), 0.0f)
		//);


		//Entity ButtonUIB = newChunk.CreateNewEntity(
		//	UIComponent("ButtonRB", float2(-0.93f, -0.65f), float2(0.15f * 0.8f, 0.275f * 0.8f), 0.0f)
		//);

		//Entity JumpUI = newChunk.CreateNewEntity(
		//	UIComponent("JumpUI", float2(-0.73f, -0.65f), float2(0.35f * 0.8f, 0.22f * 0.8f), 0.0f)
		//);



// #define CAMERA_3RD

#ifdef CAMERA_3RD
	Entity cameraRig = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.5f, 0.0f) + kDefaultWorldPosition,
			float3(30.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		CameraRigTag(),
		FollowPosition(float3(0.0f, 0.0f, 0.0f), player, FOLLOW_POS_LOCALOFFSET | FOLLOW_POS_FIXED_Y),
		FollowLeap(float3(kPlayerMoveSpeed.x, 0.0f, kPlayerMoveSpeed.y)),
		PosePosState(POSE_POS_FOLLOW),
		CameraPoint(100.0f, 100.0f, 100.0f, float3(0.0f, 0.0f, -10.5f))
	);

	Entity camera = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		Camera(100.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0f), 60.0f, 1.77777f, 0.05f, 1000.0f),
		CameraTag(),
		PosePosState(POSE_POS_CAMERA),
		PoseRotState(POSE_ROT_CAMERA)
	);

#else


	Entity cameraRig = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.5f, 0.0f) + kDefaultWorldPosition,
			float3(55.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		FollowPosition(float3(0.0f, 0.0f, 0.3f), player, FOLLOW_POS_LOCALOFFSET | FOLLOW_POS_FIXED_Y),
		FollowLeap(float3(kPlayerMoveSpeed.x, 0.0f, kPlayerMoveSpeed.y)),
		PosePosState(POSE_POS_FOLLOW),
		CameraPoint(100, 100.0f, 100.0f, float3(0.0f, 0.0f, -15.0f)),
		CameraChangeEffect("CameraChange")
	);

	Entity camera = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		Camera(100, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0f), 60.0f, 1.77777f, 0.05f, 1000.0f),
		CameraMover(cameraRig),
		CameraTag(),
		PosePosState(POSE_POS_CAMERA),
		PoseRotState(POSE_ROT_CAMERA)
	);

#endif


	Entity worldPower = newChunk.CreateNewEntity(
		WorldPower(float3(0.0f, -1.2f / 60.0f, 0.0f), float3(0.5f, 1.0f, 0.5f))
	);


	Entity floor2 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, -40.0f, 0.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(kDebugWorldSize.x, 80.0f, kDebugWorldSize.z)
		),
		ModelKey("Box"),
		BoxCollider(float3(0.0f, 30.0f, 0.0f), float3(kDebugWorldSize.x, 20.0f, kDebugWorldSize.z)),
		OBBCollider(OBB_PushOutLocked),
		floorColor
	);


	Entity floor3 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(kDebugWorldSize.x / 2.0f, kDebugWorldSize.y / 2.0f, 0.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(kWallThickness, kDebugWorldSize.y, kDebugWorldSize.z)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(0.02f, kDebugWorldSize.y, kDebugWorldSize.z)),
		OBBCollider(OBB_PushOutLocked),
		UseGhostShader()
	);

	Entity floor4 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(-kDebugWorldSize.x / 2.0f, kDebugWorldSize.y / 2.0f, 0.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(kWallThickness, kDebugWorldSize.y, kDebugWorldSize.z)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(0.02f, kDebugWorldSize.y, kDebugWorldSize.x)),
		OBBCollider(OBB_PushOutLocked),
		UseGhostShader()
	);


	Entity floor5 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, kDebugWorldSize.y / 2.0f, kDebugWorldSize.z / 2.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(kDebugWorldSize.x, kDebugWorldSize.y, kWallThickness)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(kDebugWorldSize.x, kDebugWorldSize.y, 0.02f)),
		OBBCollider(OBB_PushOutLocked),
		UseGhostShader()

	);

	Entity floor6 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, kDebugWorldSize.y / 2.0f, -kDebugWorldSize.z / 2.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(kDebugWorldSize.x, kDebugWorldSize.y, kWallThickness)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(kDebugWorldSize.x, kDebugWorldSize.y, 0.02f)),
		OBBCollider(OBB_PushOutLocked),
		UseGhostShader()
	);


	Entity sea = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, -16.0f, 10.0f) + kDefaultWorldPosition,
			float3(90.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		UIComponent("Sea", float2(), float2(300.0f, 120.0f), 0.0f, 0.15f, float2(), float2(10.0f, 10.0f)),
		SpriteComponent(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), false),
		Name("Sea"),
		UVMove(float2(0.0015f, 0.0025f))
	);


	Entity underFloor = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, -20.0f, 15.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(kDebugWorldSize.x * 10.0f, 1.0, kDebugWorldSize.z * 6.0f)
		),
		ModelKey("Box"),
		seaColor
	);

	Entity snowEffect = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 5.0f, -5.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		EfkEffectKey(kSnowEffect, true)  // ループ再生
	);

	Entity battleStart = newChunk.CreateNewEntity(
		CreateEffect(BATTLESTART, float2(0.0f, 0.4f), 0.0f, 120.0f)		
	);

	Entity white = newChunk.CreateNewEntity(
		CreateEffect(WHITEWAVEEND, float2(0.0f, 0.0f), 0.0f, 0.0f)
	);

	Entity configUI = newChunk.CreateNewEntity(
		SpawnJson("GameControllerUI", 0.0f, float3())
	);

	return newChunk;

}

void ProtoWorld::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	// 移動系の処理をあらかじめ行う
	// Physics系の処理を行う
	//WorldPowerSystem(a_chunk, a_context);
	//VelocitySystem(a_chunk, a_context); //oo
	//AngularVelocitySystem(a_chunk, a_context);
	////ColliderSystem(a_chunk, a_context);
	////ColliderCheckSystem(a_chunk, a_context);
	//SectorCheckSystem(a_chunk, a_context);
	//ColliderBackSystem(a_chunk, a_context);//o

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

	CheckAliveTargetEnemySystem(a_chunk, a_context, a_response, a_aiManager, a_serialize);

	// リザルトなどをリセット
	ResetSystem(a_chunk, a_context);
}

void ProtoWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
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

	// AI更新
	a_aiManager.WriteSystem(AISystemInfo(a_context.aiDeltaTime));
	AISenseSystem(a_chunk, a_context, a_aiManager);
	if (a_context.aiTickThisFrame)
	{
		a_aiManager.TickAI();
	}

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

	KnockbackStateSystem(a_chunk, a_context);
	LookOnStateSystem(a_chunk, a_context);
	KnockbackSystem(a_chunk, a_context);

	CursorSelectSystem(a_chunk, a_context, a_response, a_aiManager, a_serialize);

	// Effect系統を処理
	UVMoveSystem(a_chunk, a_context);
	UILerpSystem(a_chunk, a_context);
	StartUISystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);
	ZoomMoveSystem(a_chunk, a_context);
	TrailSystem(a_chunk, a_context);
	SpriteAnimationSystem(a_chunk, a_context);
	HPGaugeSystem(a_chunk, a_context);
	UIShakeSystem(a_chunk, a_context);
	SoundSystem(a_chunk, a_context);
	SpawnEfkEffectAreaSystem(a_chunk, a_context);
	SpawnEfkEffectSystem(a_chunk, a_context);
	UpdateEfkEffectSystem(a_chunk, a_context);

	// ステージ読み込み
	SpawnJsonSystem(a_chunk, a_context, a_serialize, a_aiManager);
	PlayerHealSystem(a_chunk, a_context);
	CreateEffectSystem(a_chunk, a_context);

	// 終了処理
	LifeTimeSystem(a_chunk, a_context);
	CheckAliveTargetGameOverSystem(a_chunk, a_context, a_response, a_aiManager, a_serialize);
	CheckAliveTargetEnemySystem(a_chunk, a_context, a_response, a_aiManager, a_serialize);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void ProtoWorld::InitResponse(std::unique_ptr<SystemResponse>& response)
{
	response = std::make_unique<GameSystemResponse>();
}

void ProtoWorld::HandleSystemResponse(SystemResponse& a_response)
{
	GameSystemResponse& response = dynamic_cast<GameSystemResponse&>(a_response);

	if (response.IsStageRequest())
	{
		stageRequest(response.GetStageIndex());
	}

	if (!response.IsWorldRequest()) return;

	worldRequest(response.GetWorldRequest());
}


