#include "StartupWorld.h"
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

#include <fstream>

StartupWorld::StartupWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, IEffectCacheAcquisition& a_effectCache, std::function<void(int)> a_tutorialRequest, std::function<void(std::string)> a_worldRequest, Input& a_input, ComponentsSerialize& a_serialize)
	: World(a_modelCache,
		a_uiCache,
		a_effectCache,
		a_tutorialRequest,
		a_input,
		a_serialize
	)
	, worldRequest(a_worldRequest)
{   
	a_input.RegisterButton("Select", XINPUT_GAMEPAD_A);
	a_input.RegisterKey("Select", VK_SPACE);
}

void StartupWorld::InitAI(AIManager& a_aiManager)
{
	std::unique_ptr<BehaviorTree> bt = std::make_unique<BehaviorTree>();
	RootNode& root = bt->GetRoot();

	std::unique_ptr<ReactiveSelector> fightSelector = std::make_unique<ReactiveSelector>();

	fightSelector->AddNode(std::make_unique<InAttackRangeDecorator>(std::make_unique<UseAttackAction>()));
	fightSelector->AddNode(std::make_unique<ChasePlayerAction>(1.0f));

	std::unique_ptr<ReactiveSelector> neutralSelector = std::make_unique<ReactiveSelector>();
	neutralSelector->AddNode(std::make_unique<InAttackRangeDecorator>(std::make_unique<UseAttackAction>()));
	neutralSelector->AddNode(std::make_unique<ChasePlayerAction>(0.8f));


	std::unique_ptr<ReactiveSelector> guardSelector = std::make_unique<ReactiveSelector>();
	guardSelector->AddNode(std::make_unique<GuardNode>());


	std::unique_ptr<StanceNode> stance = std::make_unique<StanceNode>(
		std::move(guardSelector),
		std::move(fightSelector),
		std::move(neutralSelector)
	);

	root.AddNode(std::move(stance));

	a_aiManager.RegisterTree("Enemy", std::move(bt));
}

Chunk StartupWorld::CreateNewChunk(AIManager& a_aiManager)
{

	Chunk newChunk;

	newChunk.CreateNewEntity(
		UIComponent("Controller", float2(0.0f, 0.25f), float2(1.0f * 0.7f, 1.4f * 0.7f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.0125f, 0.0f, 1.0f),
		FadeChange(FADE_CHANGE_FLICKER, false, 140.0f, 0.0f)
	);

	newChunk.CreateNewEntity(
		UIComponent("ControllerText", float2(0.0f, -0.4f), float2(1.15f, 0.5f), 0.0f, 0.0f),
		FadeUI(FADE_UP, 0.0125f, 0.0f, 1.0f),
		FadeChange(FADE_CHANGE_FLICKER, false, 140.0f, 0.0f)
	);

	newChunk.CreateNewEntity(
		ChunkChange(true, "Title"),
		DelayChunkChange(300.0f)
	);

	newChunk.CreateNewEntity(
		ChunkChange(true, "Title"),
		KeyChunkChange("Select", 0.0)
	);

	return newChunk;

}

void StartupWorld::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
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

void StartupWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
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



	// Effect系統を処理
	CreateEffectSystem(a_chunk, a_context);
	UVMoveSystem(a_chunk, a_context);
	UILerpSystem(a_chunk, a_context);
	StartUISystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);
	ZoomMoveSystem(a_chunk, a_context);
	TrailSystem(a_chunk, a_context);
	SpriteAnimationSystem(a_chunk, a_context);
	HPGaugeSystem(a_chunk, a_context);
	SoundSystem(a_chunk, a_context);
	SpawnEfkEffectAreaSystem(a_chunk, a_context);
	SpawnEfkEffectSystem(a_chunk, a_context);
	UpdateEfkEffectSystem(a_chunk, a_context);

	// ステージ読み込み
	SpawnJsonSystem(a_chunk, a_context, a_serialize, a_aiManager);

	// 終了処理
	LifeTimeSystem(a_chunk, a_context);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void StartupWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}


