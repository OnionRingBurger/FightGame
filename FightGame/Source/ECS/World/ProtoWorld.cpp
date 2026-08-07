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

#include <fstream>

ProtoWorld::ProtoWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, std::function<void(int)> a_tutorialRequest, std::function<void(std::string)> a_worldRequest, Input& a_input)
	: World(a_modelCache,
		a_uiCache,
		a_tutorialRequest,
		a_input)
	, worldRequest(a_worldRequest)
{
	a_input.RegisterKey("RightAttack", VK_RIGHT);
	a_input.RegisterKey("LeftAttack", VK_LEFT);
	a_input.RegisterKey("Jump", VK_SPACE);
	a_input.RegisterKey("Guard", VK_SHIFT);
	a_input.RegisterKey("TheWorld", 'E');

	a_input.RegisterKey("Default", '0');
	a_input.RegisterKey("Burn", '1');
	a_input.RegisterKey("Drop", '2');
	a_input.RegisterKey("Fly", '3');
	a_input.RegisterKey("Shake", '4');
	a_input.RegisterKey("Retro", '5');
	a_input.RegisterKey("InputMove", '6');
	a_input.RegisterKey("InputAngle", '7');
	a_input.RegisterKey("Fish", '8');
	a_input.RegisterKey("Jump", VK_SPACE);
}

void ProtoWorld::InitAI(AIManager& a_aiManager)
{
	std::unique_ptr<BehaviorTree> bt = std::make_unique<BehaviorTree>();
	RootNode& root = bt->GetRoot();

	std::unique_ptr<ReactiveSelector> fightSelector = std::make_unique<ReactiveSelector>();

	fightSelector->AddNode(std::make_unique<InAttackRangeDecorator>(std::make_unique<UseAttackAction>()));
	fightSelector->AddNode(std::make_unique<ChasePlayerAction>());

	std::unique_ptr<StanceNode> stance = std::make_unique<StanceNode>(
		std::make_unique<RunPlayerNode>(),
		std::move(fightSelector),
		std::make_unique<WaitAction>(100.0f)
	);
	root.AddNode(std::move(stance));

	a_aiManager.RegisterTree("Enemy", std::move(bt));
}

Chunk ProtoWorld::CreateNewChunk(AIManager& a_aiManager)
{
	
	Chunk newChunk;



	Entity player = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 1.0f, 0.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		ModelKey("Box"),
		InputSource(InputOrigin::Device),
		InputMove(float2(0.1f, 0.1f)),
		MoveInputResult(),
		ShooterComponent(100.0f, 300.0f),
		Ray(0.0f, 0.0f, 0.0f),
		LaserOwnerTag(),
		PlayerTag(),
		Firework(FIREWORK_NONE),
		GroundedState(),
		DeadState(),
		ActionMask(),
		Velocity(),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		OBBCollider(),
		HitInfomation(),
		JumpPower(float3(0.0f, 12.0f / 60.0f, 0.0f)),
		AttackStatus({ 
			AttackPower(0.0f, 1.7f, 160.0f, 0.5f, 0.5f, 2.0f, 15.0f, 10.0f, float3(), 12.0f),
			AttackPower(0.0f, 6.2f, 60.0f, 0.5f, 0.5f, 2.0f, 30.0f, 30.0f, float3(), 22.0f, 50.0f)
		 }),
		LookMove(900.0f / 60.0f),
		PoseRotState(POSE_ROT_LOOKMOVE),
		HitPoint(30.0f),
		AttackHitRecord(),
		GuardState(0.2f)

	);

	Entity hpBack = newChunk.CreateNewEntity(
		UIComponent("UIBack", float2(-0.58, 0.9f), float2(0.8f, 0.1f), 0.0f)
	);

	Entity hpGauge = newChunk.CreateNewEntity(
		UIComponent("UIGauge", float2(-0.58f, 0.9f), float2(0.8f, 0.05f), 0.0f),
		HPGaugeUI(player, float2(-0.58f, 0.9f), float2(0.71f, 0.05f))
	);

	Entity hpFrame = newChunk.CreateNewEntity(
		UIComponent("UIFrame", float2(-0.58, 0.9f), float2(0.8f, 0.1f), 0.0f)
	);

	Entity debugEnemy = newChunk.CreateNewEntity(
		// タグ 
		EnemyTag(),
		// 移動系
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.5f, 3.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		InputMove(float2(0.03f, 0.03f)),
		MoveInputResult(),
		InputSource(InputOrigin::AI),
		LookMove(480.0f / 60.0f),
		PoseRotState(POSE_ROT_LOOKMOVE),
		Velocity(),
		// 当たり判定
		HitInfomation(),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f)),
		OBBCollider(),
		// AI
		AIRole(4.0f, 3.0f, 1.2f, 7.0f),
		GroundedState(),
		DeadState(),
		ActionMask(),
		// ゲームルール
		HitPoint(30.0f),
		AttackStatus({
		AttackPower(0.0f, 2.5f, 160.0f, 0.01f, 0.5f, 1.0f, 30.0f, 3.0f, float3(), 30.0f, 30.0f),
		AttackPower(2.5f, 5.7f, 110.0f, 0.01f, 0.5f, 3.0f, 60.0f, 10.0f, float3(), 50.0f, 60.0f),
			}),
		AttackHitRecord(),
		// モデル
		ModelKey("Box")
	);

	a_aiManager.RegisterAI(debugEnemy, "Enemy");

	Entity cameraRig = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 52.0f, 0.0f),
			float3(30.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		FollowPosition(float3(0.0f, 0.0f, 0.0f), player, FOLLOW_POS_LOCALOFFSET | FOLLOW_POS_FIXED_Y),
		PosePosState(POSE_POS_FOLLOW),
		CameraPoint(100.0f, 100.0f, 100.0f, float3(0.0f, 0.0f, -10.0f)),
		InputRotato(float2(1.0f, 0.0f), float2(1.0f, 0.0f))
	);

	Entity camera = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.5f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		Camera(100.0f, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0f), 60.0f, 1.77777f, 0.05f, 1000.0f),
		CameraTag(),
		PosePosState(POSE_POS_CAMERA),
		PoseRotState(POSE_ROT_CAMERA)
	);

	Entity laserPoint = newChunk.CreateNewEntity(
		LaserPointTag{},
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f),
			float3(0.17f, 0.17f, 0.5f)
		),
		ModelKey("LaserPoint")
	);

	Entity worldPower = newChunk.CreateNewEntity(
		WorldPower(float3(0.0f, -0.9f / 60.0f, 0.0f))
	);
	//Entity fire = newChunk.CreateNewEntity(
	//	MOVE_AND_TRANSFORM_COMPONENT(
	//		float3(0.0f, 0.0f, 1.0f),
	//		float3(0.0f, 0.0f, 0.0f),
	//		float3(1.0f, 1.0f, 1.0f)
	//	),
	//	//ModelKey("Box"),
	//	Velocity(0.0f, 0.0f, 0.0f),
	//	//LifeTime(300.0f),
	//	Component::AlphaBlendComponent("Star", "Star")
	//);

	Entity floor = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 15.0f, 0.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(15.0f, 0.05f, 15.0f)
		),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(15.0f, 0.05f, 15.0f)),
		OBBCollider(OBB_PushOutLocked)
	);

	Entity floor2 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, -10.0f, 0.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(15.0f, 20.05f, 15.0f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(15.0f, 20.05f, 15.0f)),
		OBBCollider(OBB_PushOutLocked)
	);

	Entity floor3 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(7.5f, 7.5f, 0.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(0.05f, 15.0f, 15.0f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(0.05f, 15.0f, 15.0f)),
		OBBCollider(OBB_PushOutLocked)
	);

	Entity floor4 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(-7.5f, 7.5f, 0.0f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(0.05f, 15.0f, 15.0f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(0.05f, 15.0f, 15.0f)),
		OBBCollider(OBB_PushOutLocked)
	);


	Entity floor5 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 7.5f, 7.5f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(15.0f, 15.0f, 0.05f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(15.0f, 15.0f, 0.05f)),
		OBBCollider(OBB_PushOutLocked)
	);

	Entity floor6 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 7.5f, -7.5f) + kDefaultWorldPosition,
			float3(0.0f, 0.0f, 0.0f),
			float3(15.0f, 15.0f, 0.05f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(15.0f, 15.0f, 0.05f)),
		OBBCollider(OBB_PushOutLocked)
	);

	return newChunk;
}

void ProtoWorld::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response)
{
	// 移動系の処理をあらかじめ行う
	// Physics系の処理を行う
	WorldPowerSystem(a_chunk, a_context);
	VelocitySystem(a_chunk, a_context); //oo
	AngularVelocitySystem(a_chunk, a_context);
	ColliderSystem(a_chunk, a_context);
	ColliderCheckSystem(a_chunk, a_context);
	SectorCheckSystem(a_chunk, a_context);
	ColliderBackSystem(a_chunk, a_context);//o

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

	// ResetConsoleSystem(a_chunk, a_context);

	// 追従などの遅延系処理
	LookMoveSystem(a_chunk, a_context);
	FollowTransformSystem(a_chunk, a_context);//o
	CameraMoveSystem(a_chunk, a_context);//oo
	LatePoseSystem(a_chunk, a_context);
	// Transform適用
	TransformSystem(a_chunk, a_context);

	// リザルトなどをリセット
	ResetSystem(a_chunk, a_context);
}

void ProtoWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager)
{
	TestSystem(a_chunk, a_context);

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
	VelocitySystem(a_chunk, a_context);
	FlipSystem(a_chunk, a_context);
	LookSystem(a_chunk, a_context);
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
	FollowTransformSystem(a_chunk, a_context);
	CameraMoveSystem(a_chunk, a_context);
	LatePoseSystem(a_chunk, a_context);
	// Transform適用
	TransformSystem(a_chunk, a_context);

	// Transform等を使用するSystemを実行
	RaySystem(a_chunk, a_context);
	LaserSystem(a_chunk, a_context);
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
	CharacterAttackSystem(a_chunk, a_context);
	PlayerJumpSystem(a_chunk, a_context);
	AttackStartupSystem(a_chunk, a_context);
	AttackActionSystem(a_chunk, a_context);
	AttackWaitActionSystem(a_chunk, a_context);
	JumpPhysicsSystem(a_chunk, a_context);
	GuardSystem(a_chunk, a_context);
	GuardActionSystem(a_chunk, a_context);
	AttackHitSystem(a_chunk, a_context, a_aiManager);
	HitPointSystem(a_chunk, a_context);
	AttackHitRecordCleanupSystem(a_chunk, a_context);
	AttackInstanceEndCheckSystem(a_chunk, a_context);
	AttackInstanceResolveSystem(a_chunk, a_context, a_aiManager);
	PlayerDeadSystem(a_chunk, a_context);
	EnemyDeadSystem(a_chunk, a_context);

	// Effect系統を処理
	CreateEffectSystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);
	ZoomMoveSystem(a_chunk, a_context);
	TrailSystem(a_chunk, a_context);
	SpriteAnimationSystem(a_chunk, a_context);
	HPGaugeSystem(a_chunk, a_context);

	// 終了処理
	LifeTimeSystem(a_chunk, a_context);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void ProtoWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}


