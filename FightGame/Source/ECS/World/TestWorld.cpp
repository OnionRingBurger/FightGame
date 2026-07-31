#include "TestWorld.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"
#include "json.hpp"
#include "DebugConsole.h"
#include "ComponentsSerialize.h"

#include "Defines.h"

#include <fstream>

//#define SAFE_LOAD(parent, child, key, defaultValue) \
//#define SAFE_LOAD(parent, child, key, defaultValue) \

TestWorld::TestWorld(
	IModelCacheAcquisition& a_modelCache,
	IUICacheAcquisition& a_uiCache,
	std::function<void(int)> a_tutorialRequest,
	std::function<void(std::string)> a_worldRequest,
	Input& a_input)
	: World(a_modelCache,
		a_uiCache,
		a_tutorialRequest,
		a_input
	)
	, worldRequest(a_worldRequest)
{

}

Chunk TestWorld::CreateNewChunk(AIManager& a_aiManager)
{
	(void)a_aiManager;
	Chunk newChunk;

	//std::unique_ptr<ComponentsSerialize> serialize = std::make_unique<ComponentsSerialize>();
	//TestRegisterComponent(*serialize);

	//LoadJsonComponent(newChunk, *serialize, "testEntities");


	Entity player = newChunk.CreateNewEntity(
		PlayerTag{},
		LaserOwnerTag{},
		Velocity(0.0f, 0.0005f, 0.0f),
		InputRotato(float2{ 1.1f, 0.8f }, float2{ 1.1f, 0.8f }),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		BoxCollider(float3{ 0.0f,1.0f, 0.0f }, float3{ 1.0f,2.3f,1.0f }),
		OBBCollider(),
		ShooterComponent(3.0f, 180.0f),
		Ray(0.0f, 0.0f, 0.0f),
		HitInfomation(),
		//ShakingComponent(false, 0.0f, 0.15f, 0.4f, SHAKING_POSITION),
		//PlayerWalkTimer(20.0f),
		GhostAreaComponent(15.0f),
		CameraPoint(0.0f, 50.0f, 50.0f, float3(0.0f, 1.0f, 0.0f), float3()),
		InputMove(float2(0.05f, 0.05f))
	);

	Entity block = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 50.0f),
			float3(30.0f, 0.0f, 0.0f),
			float3(5.0f, 5.0f, 5.0f)
		),
		ModelKey("bricktexture1", float3(), float3(), MODEL_DRAW_DEFAULT),
		PoseRotState(POSE_ROT_FOLLOW),
		FollowRotation(float3(), player)
	);

	Entity camera = newChunk.CreateNewEntity(
		Camera(100, float3{ 0.0f, 0.0f, 0.0f }, float3{ 0.0f, 1.0f, 0.0f }, 60.0f, 16.0f / 9.0f, 0.05f, 1000.0f),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_CAMERA),
		PoseRotState(POSE_ROT_CAMERA),
		ZoomComponent(1.0f, 1.2f),
		ZoomMove(ZOOMMOVE_ZERORESET)
	);

	ResetSound();

	return newChunk;
}

void TestWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager)
{
	// PhysicsånÇÃèàóùÇçsÇ§
	VelocitySystem(a_chunk, a_context); //oo
	ColliderSystem(a_chunk, a_context);
	ColliderCheckSystem(a_chunk, a_context);
	ColliderBackSystem(a_chunk, a_context);//o

	// PoseånÇÃèàóùÇçsÇ§
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
	PoseSystem(a_chunk, a_context);

	// í«è]Ç»Ç«ÇÃíxâÑånèàóù
	FollowTransformSystem(a_chunk, a_context);//o
	CameraMoveSystem(a_chunk, a_context);//oo
	ShakeSystem(a_chunk, a_context);
	LatePoseSystem(a_chunk, a_context);
	// TransformìKóp
	TransformSystem(a_chunk, a_context);

	// TransformìôÇégópÇ∑ÇÈSystemÇé¿çs
	RaySystem(a_chunk, a_context);
	LaserSystem(a_chunk, a_context);
	EnemyPlayerSearch(a_chunk, a_context);
	ItemGetSystem(a_chunk, a_context);
	CameraViewSystem(a_chunk, a_context);
	BulletSystem(a_chunk, a_context, a_response);
	EnemySpawnSystem(a_chunk, a_context);
	EnemyShooterSystem(a_chunk, a_context);
	EnemyAttackSystem(a_chunk, a_context, a_response);
	PlayerDeadSystem(a_chunk, a_context);
	EnemyDeadSystem(a_chunk, a_context);
	EnemyBulletDeadSystem(a_chunk, a_context);
	GoalSystem(a_chunk, a_context);
	PlayerWalkSystem(a_chunk, a_context);
	RailUpdateSystem(a_chunk, a_context);
	HitPointSystem(a_chunk, a_context);

	// EffectånìùÇèàóù
	CreateEffectSystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);
	ZoomMoveSystem(a_chunk, a_context);
	TrailSystem(a_chunk, a_context);
	SpriteAnimationSystem(a_chunk, a_context);
	HPGaugeSystem(a_chunk, a_context);

	// èIóπèàóù
	LifeTimeSystem(a_chunk, a_context);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void TestWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}


