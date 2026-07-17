#include "ProtoWorld.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"
#include "json.hpp"
#include "DebugConsole.h"
#include "ComponentsSerialize.h"
#include "Defines.h"

#include <fstream>

ProtoWorld::ProtoWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, std::function<void(int)> a_tutorialRequest, std::function<void(std::string)> a_worldRequest, Input& a_input)
	: World(a_modelCache,
		a_uiCache,
		a_tutorialRequest,
		a_input)
	, worldRequest(a_worldRequest)
{
	a_input.RegisterKey("Shot", VK_LBUTTON);
	a_input.RegisterButton("Shot", VK_PAD_RTRIGGER);
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
}

Chunk ProtoWorld::CreateNewChunk()
{
	
	Chunk newChunk;

	Entity floor = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 15.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(15.0f, 0.05f, 15.0f)
		),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(15.0f, 0.05f, 15.0f)),
		OBBCollider()
	);

	Entity floor2 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(15.0f, 0.05f, 15.0f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(15.0f, 0.05f, 15.0f)),
		OBBCollider()
	);

	Entity floor3 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(7.5f, 7.5f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(0.05f, 15.0f, 15.0f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(0.05f, 15.0f, 15.0f)),
		OBBCollider()
	);

	Entity floor4 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(-7.5f, 7.5f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(0.05f, 15.0f, 15.0f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(0.05f, 15.0f, 15.0f)),
		OBBCollider()
	);


	Entity floor5 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 7.5f, 7.5f),
			float3(0.0f, 0.0f, 0.0f),
			float3(15.0f, 15.0f, 0.05f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(15.0f, 15.0f, 0.05f)),
		OBBCollider()
	);

	Entity floor6 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 7.5f, -7.5f),
			float3(0.0f, 0.0f, 0.0f),
			float3(15.0f, 15.0f, 0.05f)
		),
		// ModelKey("Box"),
		BoxCollider(float3(0.0f, 0.0f, 0.0f), float3(15.0f, 15.0f, 0.05f)),
		OBBCollider()
	);

	Entity player = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 1.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		ModelKey("Box"),
		InputMove(float2(0.1f,0.1f)),
		ShooterComponent(100.0f,300.0f),
		Ray(0.0f, 0.0f, 0.0f),
		LaserOwnerTag(),
		PlayerTag(),
		Firework(FIREWORK_NONE)
	);


	Entity cameraRig = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(30.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		FollowPosition(float3(0.0f, 0.0f, 0.0f), player, FOLLOW_POS_LOCALOFFSET),
		PosePosState(POSE_POS_FOLLOW),
		CameraPoint(100.0f, 100.0f, 100.0f, float3(0.0f, 0.0f, -5.0f)),
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

	return newChunk;
}

void ProtoWorld::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response)
{
	// à⁄ìÆånÇÃèàóùÇÇ†ÇÁÇ©Ç∂ÇﬂçsÇ§
	// PhysicsånÇÃèàóùÇçsÇ§
	VelocitySystem(a_chunk, a_context); //oo
	AngularVelocitySystem(a_chunk, a_context);
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
	ShakeSystem(a_chunk, a_context);
	PoseSystem(a_chunk, a_context);

	ResetConsoleSystem(a_chunk, a_context);

	// í«è]Ç»Ç«ÇÃíxâÑånèàóù
	FollowTransformSystem(a_chunk, a_context);//o
	CameraMoveSystem(a_chunk, a_context);//oo
	LatePoseSystem(a_chunk, a_context);
	// TransformìKóp
	TransformSystem(a_chunk, a_context);

	// ÉäÉUÉãÉgÇ»Ç«ÇÉäÉZÉbÉg
	ResetSystem(a_chunk, a_context);
}

void ProtoWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response)
{
	// PhysicsånÇÃèàóùÇçsÇ§
	VelocitySystem(a_chunk, a_context); //oo
	AngularVelocitySystem(a_chunk, a_context);
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
	ShakeSystem(a_chunk, a_context);
	PoseSystem(a_chunk, a_context);

	// í«è]Ç»Ç«ÇÃíxâÑånèàóù
	FollowTransformSystem(a_chunk, a_context);//o
	CameraMoveSystem(a_chunk, a_context);
	LatePoseSystem(a_chunk, a_context);
	// TransformìKóp
	TransformSystem(a_chunk, a_context);

	// TransformìôÇégópÇ∑ÇÈSystemÇé¿çs
	SetFireTypeSystem(a_chunk, a_context);
	RaySystem(a_chunk, a_context);
	LaserSystem(a_chunk, a_context);
	EnemyPlayerSearch(a_chunk, a_context);
	ItemGetSystem(a_chunk, a_context);
	CameraViewSystem(a_chunk, a_context);
	SpawnObjectSystem(a_chunk, a_context);
	RailUpdateSystem(a_chunk, a_context);
	HitPointSystem(a_chunk, a_context);
	TheWorldSystem(a_chunk, a_context, a_response);
	ForceSystem(a_chunk, a_context);

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

void ProtoWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}


