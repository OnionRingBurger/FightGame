#include "GameWorld.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"
#include "json.hpp"
#include "DebugConsole.h"
#include "ComponentsSerialize.h"
#include "Defines.h"

#include <fstream>

GameWorld::GameWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, std::function<void(int)> a_tutorialRequest, std::function<void(std::string)> a_worldRequest, Input& a_input)
	: World(a_modelCache,
		a_uiCache,
		a_tutorialRequest,
		a_input)
	, worldRequest(a_worldRequest)
{
	a_input.RegisterKey("Shot", VK_LBUTTON);
	a_input.RegisterButton("Shot", VK_PAD_RTRIGGER);
}

Chunk GameWorld::CreateNewChunk()
{

	Chunk newChunk;

	std::unique_ptr<ComponentsSerialize> serialize = std::make_unique<ComponentsSerialize>();
	TestRegisterComponent(*serialize);

	//LoadJsonComponent(newChunk, *serialize, "gameEntities");

	ResetSound();
	PlaySound(LoadSound("Assets/Sound/gamebgm.mp3", true));
	PlaySound(LoadSound("Assets/Sound/minecar.mp3", true));

	Entity mainRail = newChunk.CreateNewEntity(
		RailComponent(true, float3(0.0f, kRailPosition, 25.0f), float3(0.0f, kTonnelHeight / 2.5, kTonnelLength), 3000.0f)
	);

	Entity player = newChunk.CreateNewEntity(
		PlayerTag{},
		LaserOwnerTag{},
		InputRotato(float2(1.1f, 1.1f * SCREEN_HEIGHT / SCREEN_WIDTH), float2(1.8f, 1.8f * SCREEN_HEIGHT / SCREEN_WIDTH)),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		BoxCollider(float3{ 0.0f,1.0f, 0.0f }, float3{ 1.0f,2.3f,1.0f }),
		OBBCollider(),
		ShooterComponent(3.0f, 60.0f),
		Ray(0.0f, 0.0f, 0.0f),
		HitInfomation(),
		PlayerWalkTimer(20.0f),
		//GhostAreaComponent(15.0f),
		CameraPoint(0.0f, 50.0f, 5000.0f, float3(0.0f, 1.0f, 0.0f), float3()),
		PosePosState(POSE_POS_RAIL),
		RailUser(mainRail),
		AngleLimitComponent(true, true, 70.0f, -70.0f, 30.0f, -70.0f),
		HitPoint(300.0f),
		DeadState()
	);

	Entity enemy = newChunk.CreateNewEntity();

	Entity mineCar = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, kMineCarModelPosition, 0.0f),
			float3(0.0f, -90.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_RAIL),
		RailUser(mainRail),
		ShakeComponent(float3(0.0f, 0.005f, 0.0f), float3(0.0f, 0.5f, 0.0f), 999999.0f),
		ModelKey("Minecart")
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


	Entity shotFadeIcon = newChunk.CreateNewEntity(
		UIComponent("ShotIcon", float2(0.85f, -0.75f), float2(0.17f, 0.3f), 25.0f, 0.0f),
		FadeUI(FADE_DOWN, 0.018f),
		FadeChange(FADE_CHANGE_FLICKER),
		ShotUI(SHOTUI_WAIT | SHOTUI_FADERESET_DOWN)
	);

	Entity shotIcon = newChunk.CreateNewEntity(
		UIComponent("ShotIcon", float2(0.85f, -0.75f), float2(0.17f, 0.3f), 25.0f),
		ShotUI(SHOTUI_CANSHOT)
	);

	Entity waitShot = newChunk.CreateNewEntity(
		UIComponent("WaitShot", float2(0.85f, -0.9f), float2(0.12f, 0.12f), 0.0f, 1.0f, float2(0.0f, 0.0f), float2(1.0f, 1.0f), true),
		FadeUI(FADE_DOWN, 0.018f),
		FadeChange(FADE_CHANGE_FLICKER),
		ShotUI(SHOTUI_WAIT | SHOTUI_FADERESET_DOWN)
	);

	Entity canShotReady = newChunk.CreateNewEntity(
		UIComponent("CanShotReady", float2(0.85f, -0.9f), float2(0.14f, 0.12f), 0.0f, 1.0f, float2(0.0f, 0.0f), float2(1.0f, 1.0f), true),
		FadeUI(FADE_DOWN, 0.018f),
		FadeChange(FADE_CHANGE_FLICKER),
		ShotUI(SHOTUI_CANSHOT | SHOTUI_FADERESET_DOWN)
	);

	Entity shotKeyIcon = newChunk.CreateNewEntity(
		UIComponent("LeftClick", float2(0.85f, -0.6f), float2(0.06f, 0.08f), 0.0f, 1.0f, float2(0.0f, 0.0f), float2(1.0f, 1.0f), true),
		ShotUI(SHOTUI_CANSHOT)
	);

	Entity laserPoint = newChunk.CreateNewEntity(
		LaserPointTag{},
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f),
			float3(0.17f, 0.17f, 0.5f)
		),
		ModelKey("LaserPoint"),
		GhostAreaComponent(2.0f)
		);

	Entity laser = newChunk.CreateNewEntity(
		LaserTag{},
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f),
			float3(0.3f, 0.3f, 100.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		LookLaserPoint(player, 10.0f, float3(0.7f, -1.3f, 7.7f)),
		ModelKey("Laser"),
		FollowPosition(0.7f, -1.3f, 7.7f, player, FOLLOW_POS_LOCALOFFSET)
		);

	Entity rifle = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		ModelKey("assaultRifle", float3(), { -90.0f, -0.0f, 0.0f }),
		LookLaserPoint(player, 10.0f, float3(0.7f, -1.5f, 4.7f)),
		GunTag{},
		FollowPosition(0.7f, -1.5f, 4.7f, player, FOLLOW_POS_LOCALOFFSET)
		);
	
	Entity camera = newChunk.CreateNewEntity(
		Camera(100, float3{ 0.0f, 30.0f, 30.0f }, float3{ 0.0f, 1.0f, 0.0f }, 60.0f, 16.0f / 9.0f, 0.05f, 1000.0f),
		CameraTag(),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_CAMERA),
		PoseRotState(POSE_ROT_CAMERA),
		ZoomComponent(1.0f, 1.2f),
		ZoomMove(ZOOMMOVE_ZERORESET),
		ShakeComponent(float3(0.1f, 0.05f, 0.0f), float3(0.5f, 0.5f, 0.0f))
	);


	Entity gameStart = newChunk.CreateNewEntity(
		UIComponent("GameStart", float2(0.0f, 0.37f), float2(1.0f, 1.0f), 0.0f),
		FadeUI(FADE_DOWN, 0.006f),
		LifeTime(180.0f)
	);


	Entity spawner = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		FollowPosition(0.0f, 0.0f, 0.0f, player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y),
		EnemySpawner(290.0f, SPAWN_ENEMY_FORWARD)
	);

	Entity spawner2 = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		FollowPosition(0.0f, 0.0f, 0.0f, player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y),
		EnemySpawner(600.0f, SPAWN_ENEMY_SIDE)
	);

	Entity specialSpawner = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		FollowPosition(0.0f, 0.0f, 0.0f, player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y),
		EnemySpawner(500.0f, SPAWN_ENEMY_SHOOTER | SPAWN_ENEMY_GROUP)
	);

	Entity dark = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(30.0f, 30.0f, 0.1f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(kTonnelWidth - 0.5f, kTonnelHeight - 0.5f, 0.1f)),
		OBBCollider(OBB_TRIGGER),
		ModelKey("Dark", float3(), float3(), MODEL_DRAW_SHADOW),
		FollowPosition(0.0f, 0.0f, -30.0f, player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y),
		DarkTag()
	);

	Entity forwardDark = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(kTonnelWidth, kTonnelHeight, 0.1f)
		),
		PosePosState(POSE_POS_FOLLOW),
		FollowPosition(0.0f, 0.0f, kTonnelSegmentLength * (kTonnelSegmentCount - 2), player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y),
		ModelKey("Dark", float3(), float3(), MODEL_DRAW_SHADOW)
	);

	Entity goal = newChunk.CreateNewEntity(
		TRANSFORM_COMPONENT(
			float3(0.0f, kTonnelHeight / 2, kTonnelLength),
			float3(0.0f, 0.0f, 0.0f),
			float3(kTonnelWidth, kTonnelHeight, 0.1f)
		),
		BoxCollider(float3(), float3(kTonnelWidth - 0.5f, kTonnelHeight - 0.5f, 1.0f)),
		OBBCollider(OBB_TRIGGER),
		ModelKey("Light", float3(), float3(), MODEL_DRAW_SHADOW),
		GoalTag{}
	);

	Entity wallCollider1 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(kTonnelWidth / 2, kTonnelHeight / 2, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(1.0f, kTonnelHeight, kTonnelColliderLength)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(float3(), player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity wallCollider2 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(-kTonnelWidth / 2, kTonnelHeight / 2, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(1.0f, kTonnelHeight, kTonnelColliderLength)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(float3(), player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity wallCollider3 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(kTonnelWidth, 1.0, kTonnelColliderLength)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(float3(), player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity wallCollider4 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, kTonnelHeight, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(kTonnelWidth, 1.0f, kTonnelColliderLength)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(float3(), player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity wallCollider5 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, kTonnelHeight / 2, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(kTonnelWidth, kTonnelHeight, 1.0f)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(float3(0.0f, 0.0f, kTonnelColliderLength / 2.0f), player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity wallCollider6 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, kTonnelHeight / 2, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(kTonnelWidth, kTonnelHeight, 1.0f)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(float3(0.0f, 0.0f, -kTonnelColliderLength / 2.0f), player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity wallCollider7 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(-kTonnelWidth / 2.5f, kTonnelHeight / 2.0f + kTonnelHeight / 2.5f, 0.0f),
			float3(0.0f, 0.0f, 45.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(kTonnelWidth, 1.0f, kTonnelColliderLength)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(0.0f, 0.0f, 0.0f, player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity wallCollider8 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(kTonnelWidth / 2.5f, kTonnelHeight / 2.0f + kTonnelHeight / 2.5f, 0.0f),
			float3(0.0f, 0.0f, 315.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(kTonnelWidth, 1.0f, kTonnelColliderLength)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(0.0f, 0.0f, 0.0f, player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity wallCollider9 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(kTonnelWidth / 2.5f, kTonnelHeight / 2.0f - kTonnelHeight / 2.5f, 0.0f),
			float3(0.0f, 0.0f, 45.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(kTonnelWidth, 1.0f, kTonnelColliderLength)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(0.0f, 0.0f, 0.0f, player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity wallCollider10 = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(-kTonnelWidth / 2.5f, kTonnelHeight / 2.0f - kTonnelHeight / 2.5f, 0.0f),
			float3(0.0f, 0.0f, 315.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(kTonnelWidth, 1.0f, kTonnelColliderLength)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(0.0f, 0.0f, 0.0f, player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity railCollider = newChunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(-0.1f, kTonnelHeight / 2.0f - kTonnelHeight / 4.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_FOLLOW),
		BoxCollider(float3(), float3(12.0f, 10.0f, kTonnelColliderLength)),
		OBBCollider(OBB_PushOutLocked),
		FollowPosition(0.0f, 0.0f, 0.0f, player, FOLLOW_POS_FIXED_X | FOLLOW_POS_FIXED_Y)
	);

	Entity startUI = newChunk.CreateNewEntity(
		UIComponent("Mask", float2(0.0f, 0.0f), float2(2.0f, 2.0f), 0.0f)
	);

	Entity createGameWhiteFade = newChunk.CreateNewEntity(
		CreateEffect(WHITEFADE_CLEAR)
	);

	for (int i = 0; i < kTonnelSegmentCount; i++)
	{
		Entity tonnel = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, kTonnelHeight / 2, kTonnelSegmentLength * i),
				float3(0.0f, 0.0f, 0.0f),
				float3(0.1f, 0.1f, 0.1f)
			),
			TrackingWarp(
				player,
				float3(0.0f, 0.0f, -kTonnelSegmentLength),
				float3(0.0f, 0.0f, kTonnelSegmentLength * (kTonnelSegmentCount) - kTonnelWarpForwardMargin),
				TRACK_FIXED_X | TRACK_FIXED_Y | TRACK_NONTRACK)
		);

		Entity floor1 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 0.0f, 0.0f),
				float3(kTonnelWidth, 1.0f, kTonnelSegmentLength)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(0.0f, -kTonnelHeight / 2.0f, 0.0f, tonnel)
		);

		Entity floor2 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 0.0f, 0.0f),
				float3(kTonnelWidth, 1.0f, kTonnelSegmentLength)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(0.0f, kTonnelHeight / 2.0f, 0.0f, tonnel)
		);

		Entity floor3 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 0.0f, 0.0f),
				float3(1.0f, 60.0f, kTonnelSegmentLength)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(kTonnelWidth / 2.0f, 0.0f, 0.0f, tonnel)
		);

		Entity floor4 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 0.0f, 0.0f),
				float3(1.0f, kTonnelHeight, kTonnelSegmentLength)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(-kTonnelWidth / 2.0f, 0.0f, 0.0f, tonnel)
		);

		Entity floor5 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 0.0f, 45.0f),
				float3(1.0f, kTonnelHeight, kTonnelSegmentLength)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(kTonnelWidth / 2.5f, kTonnelHeight / 2.5f, 0.0f, tonnel)
		);

		Entity floor6 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 0.0f, 225.0f),
				float3(60.0f, 1.0f, kTonnelSegmentLength)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(-kTonnelWidth / 2.5f, kTonnelHeight / 2.5f, 0.0f, tonnel)
		);

		Entity floor7 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 0.0f, 45.0f),
				float3(1.0f, 60.0f, kTonnelSegmentLength)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(-kTonnelWidth / 2.5f, -kTonnelHeight / 2.5f, 0.0f, tonnel)
		);

		Entity floor8 = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 0.0f, 225.0f),
				float3(60.0f, 1.0f, kTonnelSegmentLength)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(kTonnelWidth / 2.5f, -kTonnelHeight / 2.5f, 0.0f, tonnel)
		);

		Entity railFloor = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 180.0f, 0.0f),
				float3(30.0f, 35.0f, kTonnelSegmentLength)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("bricktexture1"),
			HitInfomation(),
			FollowPosition(0.0f, -kTonnelHeight / 2.0f, 0.0f, tonnel)
		);

		if (i % 2 != 0) continue;

		Entity P = newChunk.CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(90.0f, 0.0f, 0.0f),
				float3(1.0f, 3.15f, 0.8f)
			),
			PosePosState(POSE_POS_FOLLOW),
			ModelKey("track_txt"),
			HitInfomation(),
			FollowPosition(1.5f, -kTonnelHeight / 3.0, kRailModelPosition, tonnel)
		);
		
	}

	//nlohmann::ordered_json json;
	//json["testEntities"] = OutJsonComponents(newChunk, *serialize);

	//std::ofstream file(kDataTestPath);
	//if (file.is_open())
	//{
	//	file << json.dump(4);
	//}

	return newChunk;
}

void GameWorld::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response)
{
	// ˆÚ“®Œn‚Ìˆ—‚ð‚ ‚ç‚©‚¶‚ßs‚¤
	// PhysicsŒn‚Ìˆ—‚ðs‚¤
	VelocitySystem(a_chunk, a_context); //oo
	ColliderSystem(a_chunk, a_context);
	ColliderCheckSystem(a_chunk, a_context);
	ColliderBackSystem(a_chunk, a_context);//o

	// PoseŒn‚Ìˆ—‚ðs‚¤
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

	// ’Ç]‚È‚Ç‚Ì’x‰„Œnˆ—
	FollowTransformSystem(a_chunk, a_context);//o
	CameraMoveSystem(a_chunk, a_context);//oo
	LatePoseSystem(a_chunk, a_context);
	// Transform“K—p
	TransformSystem(a_chunk, a_context);

	// ƒŠƒUƒ‹ƒg‚È‚Ç‚ðƒŠƒZƒbƒg
	ResetSystem(a_chunk, a_context);
}

void GameWorld::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response)
{
	// PhysicsŒn‚Ìˆ—‚ðs‚¤
	VelocitySystem(a_chunk, a_context); //oo
	ColliderSystem(a_chunk, a_context);
	ColliderCheckSystem(a_chunk, a_context);
	SectorCheckSystem(a_chunk, a_context);
	ColliderBackSystem(a_chunk, a_context);//o

	// PoseŒn‚Ìˆ—‚ðs‚¤
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

	// ’Ç]‚È‚Ç‚Ì’x‰„Œnˆ—
	FollowTransformSystem(a_chunk, a_context);//o
	CameraMoveSystem(a_chunk, a_context);
	LatePoseSystem(a_chunk, a_context);
	// Transform“K—p
	TransformSystem(a_chunk, a_context);

	// Transform“™‚ðŽg—p‚·‚éSystem‚ðŽÀs
	RaySystem(a_chunk, a_context);
	LaserSystem(a_chunk, a_context);
	EnemyPlayerSearch(a_chunk, a_context);
	PlayerAttackSystem(a_chunk, a_context);
	ItemGetSystem(a_chunk, a_context);
	CameraViewSystem(a_chunk, a_context);
	BulletSystem(a_chunk, a_context, a_response);
	EnemySpawnSystem(a_chunk, a_context);
	EnemyShooterSystem(a_chunk, a_context);
	EnemyAttackSystem(a_chunk, a_context, a_response);
	PlayerDeadSystem(a_chunk, a_context);
	EnemyAttackHitSystem(a_chunk, a_context);
	EnemyDeadSystem(a_chunk, a_context);
	EnemyBulletDeadSystem(a_chunk, a_context);
	GoalSystem(a_chunk, a_context);
	PlayerWalkSystem(a_chunk, a_context);
	RailUpdateSystem(a_chunk, a_context);
	HitPointSystem(a_chunk, a_context);

	// EffectŒn“‚ðˆ—
	CreateEffectSystem(a_chunk, a_context);
	FadeUISystem(a_chunk, a_context);
	ZoomMoveSystem(a_chunk, a_context);
	TrailSystem(a_chunk, a_context);
	SpriteAnimationSystem(a_chunk, a_context);
	HPGaugeSystem(a_chunk, a_context);

	// I—¹ˆ—
	LifeTimeSystem(a_chunk, a_context);
	AttackHitRecordCleanupSystem(a_chunk, a_context);
	ResetSystem(a_chunk, a_context);
	ChunkChangeSystem(a_chunk, a_context, a_response);
}

void GameWorld::HandleSystemResponse(SystemResponse& a_response)
{
	if (!a_response.IsWorldRequest()) return;

	worldRequest(a_response.GetWorldRequest());
}


