#pragma once
#include <string>
#include <array>
#include <vector>
#include <limits>
#include <unordered_set>
#include "imgui.h"
#include "MathAssist.h"
#include "ECSTypes.h"
#include "TrailEffect.h"


#define TRANSFORM_COMPONENT(pos, rot, scale) \
	Position(pos), \
	Rotation((rot).x, (rot).y, (rot).z), \
	Scale((scale).x, (scale).y, (scale).z)

#define MOVE_AND_TRANSFORM_COMPONENT(pos, rot, scale) \
	Position(pos), \
	Rotation((rot).x, (rot).y, (rot).z), \
	Scale((scale).x, (scale).y, (scale).z), \
	Pose(), \
	MotionTransform(pos, rot), \
	FixedResult(), \
	MotionResult(), \
	EphemeralResult()
// コンポーネント展開マクロ
// !!!New!!!
#define COMPONENT_TYPE_LIST(X) \
	X(PlayerTag) \
	X(EnemyTag) \
	X(DarkTag) \
	X(GoalTag) \
	X(CameraTag) \
	X(ItemTag) \
	X(LaserPointTag) \
	X(LaserOwnerTag) \
	X(LaserTag) \
	X(PlayerViewTag) \
	X(DontHitRayTag) \
	X(GunTag) \
	X(Position) \
	X(Rotation) \
	X(Scale) \
	X(LookLaserPoint) \
	X(InputMove) \
	X(InputRotato) \
	X(ShakingComponent) \
	X(PlayerWalkTimer) \
	X(EnemySpawner) \
	X(Timer) \
	X(LookComponent) \
	X(Velocity) \
	X(Force) \
	X(BoxCollider) \
	X(MoveForward) \
	X(ShooterComponent) \
	X(GhostAreaComponent) \
	X(OBBCollider) \
	X(TrackingWarp) \
	X(HitInfomation) \
	X(ChunkChange) \
	X(KeyChunkChange) \
	X(CreateEffect) \
	X(ZoomComponent) \
	X(ZoomMove) \
	X(DelayChunkChange) \
	X(FadeUI) \
	X(FadeChange) \
	X(OwnerComponent) \
	X(BulletComponent) \
	X(Camera) \
	X(CameraPoint) \
	X(FollowPosition) \
	X(FollowRotation) \
	X(Ray) \
	X(RayInfomation) \
	X(LifeTime) \
	X(TrailComponent) \
	X(ModelKey) \
	X(UIComponent) \
	X(SpriteComponent) \
	X(Poliline) \
	X(LeapPosComponent) \
	X(LeapRotComponent) \
	X(FlipComponent) \
	X(UIAngularSpeed) \
	X(Pose) \
	X(PosePosState) \
	X(PoseRotState) \
	X(RailComponent) \
	X(RailUser) \
	X(RailFly) \
	X(AngleLimitComponent) \
	X(DebugCameraTag) \
	X(DebugInputPos) \
	X(DebugInputRot) \
	X(EnemyShooter) \
	X(EnemyBulletTag) \
	X(HitPoint) \
	X(DeadState) \
	X(HPGaugeUI) \
	X(ShakeComponent) \
	X(AddDamageComponent) \
	X(SpriteAnimation) \
	X(ShotUI) \
	X(UVMove) \
	X(MotionTransform) \
	X(FixedResult) \
	X(MotionResult) \
	X(EphemeralResult) \
	X(Firework) \
	X(AlphaBlendComponent) \
	X(AngularVelocity) \
	X(SectorHitJudge) \
	X(PlayerAttackTag) \
	X(AttackHitRecord) \
	X(GroundedState) \
	X(ActionMask) \
	X(JumpAction) \
	X(WorldPower) \
	X(JumpPower) \
	X(AttackStatus) \
	X(AttackAction) \
	X(AttackWaitAction) \
	X(LookMove) \
	X(MoveInputResult) \
	X(InputSource) \
	X(EnemyAttackTag) \
	X(AttackTelegraph) \
	X(AttackStartupAction) \
	X(AttackInstance) \
	X(AIRole) \
	X(GuardAction) \
	X(GuardState) \
	X(ClearTarget) \
	X(EffectKey) \
	X(KnockbackAction) \
	X(InterferenceResult) \
	X(Name) \
	X(LookOnState) \
	X(LookOnAction)


using BitFlag = unsigned int;
constexpr Entity kInvalidEntity = {
	-1,
	-1
};


// ・ｽT・ｽ・ｽ・ｽ竄ｷ・ｽ・ｽ・ｽ謔､・ｽ・ｽnamespace・ｽﾅ囲ゑｿｽ・ｽA・ｽ・ｽ{・ｽ・ｽusing namespace・ｽ・ｽ・ｽg・ｽ・ｽ
namespace Component
{
	struct PlayerTag
	{
		static constexpr TypeID kTypeId = 0;
		static constexpr const char* kTypeName = "PlayerTag";
		static constexpr int kVersion = 0;

		PlayerTag() {};
	};

	struct EnemyTag
	{
		static constexpr TypeID kTypeId = 1;
		static constexpr const char* kTypeName = "EnemyTag";
		static constexpr int kVersion = 0;
		EnemyTag() {};
	};

	struct DarkTag
	{
		static constexpr TypeID kTypeId = 2;
		static constexpr const char* kTypeName = "DarkTag";
		static constexpr int kVersion = 0;
		DarkTag() {};
	};

	struct GoalTag
	{
		static constexpr TypeID kTypeId = 3;
		static constexpr const char* kTypeName = "GoalTag";
		static constexpr int kVersion = 0;
		GoalTag() {};
	};

	struct CameraTag
	{
		static constexpr TypeID kTypeId = 4;
		static constexpr const char* kTypeName = "CameraTag";
		static constexpr int kVersion = 0;
		CameraTag() {};
	};

	struct ItemTag
	{
		static constexpr TypeID kTypeId = 5;
		static constexpr const char* kTypeName = "ItemTag";
		static constexpr int kVersion = 0;
		ItemTag() {};
	};

	struct LaserPointTag
	{
		static constexpr TypeID kTypeId = 6;
		static constexpr const char* kTypeName = "LaserPointTag";
		static constexpr int kVersion = 0;
		LaserPointTag() {};
	};

	struct LaserOwnerTag
	{
		static constexpr TypeID kTypeId = 7;
		static constexpr const char* kTypeName = "LaserOwnerTag";
		static constexpr int kVersion = 0;
		LaserOwnerTag() {};
	};

	struct LaserTag
	{
		static constexpr TypeID kTypeId = 8;
		static constexpr const char* kTypeName = "LaserTag";
		static constexpr int kVersion = 0;
		LaserTag() {};
	};

	struct PlayerViewTag
	{
		static constexpr TypeID kTypeId = 9;
		static constexpr const char* kTypeName = "PlayerViewTag";
		static constexpr int kVersion = 0;
		PlayerViewTag() {};
	};

	struct DontHitRayTag
	{
		static constexpr TypeID kTypeId = 10;
		static constexpr const char* kTypeName = "DontHitRayTag";
		static constexpr int kVersion = 0;
		DontHitRayTag() = default;
	};

	struct GunTag
	{
		static constexpr TypeID kTypeId = 11;
		static constexpr const char* kTypeName = "GunTag";
		static constexpr int kVersion = 0;
		GunTag() = default;
	};


	struct Position
	{
		static constexpr TypeID kTypeId = 12;
		static constexpr const char* kTypeName = "Position";
		static constexpr int kVersion = 0;

		float x;
		float y;
		float z;

		Position()
			: Position(0.0f, 0.0f, 0.0f)
		{
		}

		Position(float a_x, float a_y, float a_z)
			: x(a_x)
			, y(a_y)
			, z(a_z)
		{
		}

		Position(float3 a_pos)
			: x(a_pos.x)
			, y(a_pos.y)
			, z(a_pos.z)
		{
		}
	};

	struct Rotation
	{
		static constexpr TypeID kTypeId = 13;
		static constexpr const char* kTypeName = "Rotation";
		static constexpr int kVersion = 1;

		float pitch;
		float yaw;
		float roll;

		Rotation()
			: Rotation(0.0f, 0.0f, 0.0f)
		{
		}

		Rotation(float a_x, float a_y, float a_z)
			: pitch(a_x)
			, yaw(a_y)
			, roll(a_z)
		{
		}

		Rotation(float3 a_rot)
			: pitch(a_rot.x)
			, yaw(a_rot.y)
			, roll(a_rot.z)
		{
		}
	};


	struct Scale
	{
		static constexpr TypeID kTypeId = 14;
		static constexpr const char* kTypeName = "Scale";
		static constexpr int kVersion = 0;

		float x;
		float y;
		float z;

		Scale()
			: Scale(0.0f, 0.0f, 0.0f)
		{
		}

		Scale(float a_x, float a_y, float a_z)
			: x(a_x)
			, y(a_y)
			, z(a_z)
		{
		}

		Scale(float3 a_scale)
			: x(a_scale.x)
			, y(a_scale.y)
			, z(a_scale.z)
		{
		}
	};

	struct LookLaserPoint
	{
		static constexpr TypeID kTypeId = 15;
		static constexpr const char* kTypeName = "LookLaserPoint";
		static constexpr int kVersion = 0;

		Entity laserOwner;
		float minDistance;
		float3 offset;

		LookLaserPoint()
			: LookLaserPoint(kInvalidEntity, 0.0f, float3())
		{
		}

		LookLaserPoint(Entity a_laserOwner, float a_maxDistance, float3 a_offset)
			:laserOwner(a_laserOwner)
			, minDistance(a_maxDistance)
			, offset(a_offset)
		{
		}
	};
	struct InputMove
	{
		static constexpr TypeID kTypeId = 16;
		static constexpr const char* kTypeName = "InputMove";
		static constexpr int kVersion = 0;

		float2 moveSpeed;

		InputMove()
			: InputMove(float2())
		{
		}

		InputMove(float2 a_moveSpeed)
			:moveSpeed(a_moveSpeed)
		{
		}
	};

	struct InputRotato
	{
		static constexpr TypeID kTypeId = 17;
		static constexpr const char* kTypeName = "InputRotato";
		static constexpr int kVersion = 1;

		float2 mouseRotatoSpeed;
		float2 padRotatoSpeed;

		InputRotato()
			: InputRotato(float2(), float2())
		{
		}

		InputRotato(float2 a_mouseRotatoSpeed, float2 a_padRotatoSpeed)
			: mouseRotatoSpeed(a_mouseRotatoSpeed)
			, padRotatoSpeed(a_padRotatoSpeed)
		{
		}
	};


	enum ShakingType
	{
		SHAKING_POSITION,
		SHAKING_MODEL,
		MAX_SHAKING
	};
	struct ShakingComponent
	{
		static constexpr TypeID kTypeId = 18;
		static constexpr const char* kTypeName = "ShakingComponent";
		static constexpr int kVersion = 0;

		bool isWait;
		float shankingDegree;
		float heightOffset;
		float shakingSpeed;
		float shakingDisplacement;
		ShakingType shakingType;

		ShakingComponent()
			: ShakingComponent(false, 0.0f, 0.0f, 0.0f, SHAKING_POSITION)
		{
		}

		ShakingComponent(bool a_isWait, float a_heightOffset, float a_shakingSpeed, float a_shakingDisplament, ShakingType a_shakingType)
			: shankingDegree(0.0f)
			, isWait(a_isWait)
			, heightOffset(a_heightOffset)
			, shakingSpeed(a_shakingSpeed)
			, shakingDisplacement(a_shakingDisplament)
			, shakingType(a_shakingType)
		{
		}
	};

	struct PlayerWalkTimer
	{
		static constexpr TypeID kTypeId = 19;
		static constexpr const char* kTypeName = "PlayerWalkTimer";
		static constexpr int kVersion = 0;

		float maxTime;
		float time;
		PlayerWalkTimer()
			: PlayerWalkTimer(0.0f)
		{
		}

		PlayerWalkTimer(float a_maxTime)
			: maxTime(a_maxTime)
			, time(0.0f)
		{
		}
	};

	enum SpawnType
	{
		SPAWN_NORMAL_FORWARD_1,
		SPAWN_NORMAL_FORWARD_2,
		SPAWN_NORMAL_FORWARD_3,
		SPAWN_NORMAL_LEFT_1,
		SPAWN_NORMAL_RIGHT_1,
		SPAWN_BODYSLAM_FOWERD_1,
		SPAWN_BODYSLAM_FOWERD_2,
		SPAWN_STANDBY_LEFT_1,
		SPAWN_STANDBY_RIGHT_1,
		SPAWN_STANDBY_UP_1,
		SPAWN_SHOOTER_1,
		SPAWN_GROUP_1,
		MAX_SPAWN_TYPE,
	};

	enum UseSpawnEnemyFLAG
	{
		SPAWN_ENEMY_DEFAULT = 0,
		SPAWN_ENEMY_FORWARD = 1 << 0,
		SPAWN_ENEMY_SIDE = 1 << 1,
		SPAWN_ENEMY_BODYSLAM = 1 << 2,
		SPAWN_ENEMY_STANDBY = 1 << 3,
		SPAWN_ENEMY_SHOOTER = 1 << 4,
		SPAWN_ENEMY_GROUP = 1 << 5

	};

	struct EnemySpawner
	{
		static constexpr TypeID kTypeId = 20;
		static constexpr const char* kTypeName = "EnemySpawner";
		static constexpr int kVersion = 0;

		float spawnTime;
		float maxSpawnTime;
		BitFlag flag;


		EnemySpawner()
			: EnemySpawner(0.0f, SPAWN_ENEMY_DEFAULT)
		{
		}

		EnemySpawner(float a_maxSpawnTime, BitFlag a_flag)
			: maxSpawnTime(a_maxSpawnTime)
			, spawnTime(a_maxSpawnTime)
			, flag(a_flag)
		{
		}
	};

	struct Timer
	{
		static constexpr TypeID kTypeId = 21;
		static constexpr const char* kTypeName = "Timer";
		static constexpr int kVersion = 0;

		float time;
		float maxTime;

		Timer()
			: Timer(0.0f, 0.0f)
		{
		};

		Timer(float a_time, float a_maxTime)
			: time(a_time)
			, maxTime(a_maxTime)
		{
		};
	};

	struct LookComponent
	{
		static constexpr TypeID kTypeId = 22;
		static constexpr const char* kTypeName = "LookComponent";
		static constexpr int kVersion = 0;

		Entity target;
		LookComponent()
			: LookComponent(kInvalidEntity)
		{
		}

		LookComponent(Entity a_target)
			: target(a_target)
		{
		}
	};

	struct Velocity
	{
		static constexpr TypeID kTypeId = 23;
		static constexpr const char* kTypeName = "Velocity";
		static constexpr int kVersion = 0;

		float x;
		float y;
		float z;

		Velocity(float3 a_velocity)
			: x(a_velocity.x)
			, y(a_velocity.y)
			, z(a_velocity.z)
		{
		};

		Velocity(float a_x, float a_y, float a_z)
			: x(a_x)
			, y(a_y)
			, z(a_z)
		{
		}

		Velocity()
			: x(0.0f)
			, y(0.0f)
			, z(0.0f)
		{
		};
	};

	struct Force
	{
		static constexpr TypeID kTypeId = 24;
		static constexpr const char* kTypeName = "Force";
		static constexpr int kVersion = 1;

		float3 force;
		// ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ
		float attenuation;

		Force(float3 a_force, float a_attenuation = 1.0f)
			: force(a_force)
			, attenuation(a_attenuation)
		{
		}

		Force()
			: Force(float3(0.0f, 0.0f, 0.0f), 1.0f)
		{
		}

	};

	struct BoxCollider
	{
		static constexpr TypeID kTypeId = 25;
		static constexpr const char* kTypeName = "BoxCollider";
		static constexpr int kVersion = 0;

		float3 offset;
		float3 collisionScale;

		BoxCollider()
			: BoxCollider(float3(), float3())
		{
		}

		BoxCollider(float3 a_offset, float3 a_collisionScale)
			: offset(a_offset)
			, collisionScale(a_collisionScale)
		{
		}
	};

	struct MoveForward
	{
		static constexpr TypeID kTypeId = 26;
		static constexpr const char* kTypeName = "MoveForward";
		static constexpr int kVersion = 0;

		float speed;
		// ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ
		float attenuation;

		MoveForward()
			: MoveForward(0.0f, 1.0f)
		{
		}

		MoveForward(float a_speed, float a_attenuation = 1.0f)
			: speed(a_speed)
			, attenuation(a_attenuation)
		{
		}
	};

	struct ShooterComponent
	{
		static constexpr TypeID kTypeId = 27;
		static constexpr const char* kTypeName = "ShooterComponent";
		static constexpr int kVersion = 1;

		float shotSpeed;
		float shotTimer;
		float maxShotTimer;
		bool canShot;

		ShooterComponent()
			: ShooterComponent(0.0f, 0.0f)
		{
		}

		ShooterComponent(float a_shotSpeed, float a_maxShotTimer)
			: shotSpeed(a_shotSpeed)
			, shotTimer(a_shotSpeed)
			, maxShotTimer(a_maxShotTimer)
			, canShot(false)
		{
		}
	};

	struct GhostAreaComponent
	{
		static constexpr TypeID kTypeId = 28;
		static constexpr const char* kTypeName = "GhostAreaComponent";
		static constexpr int kVersion = 0;

		float length;

		GhostAreaComponent()
			: GhostAreaComponent(0.0f)
		{
		}

		GhostAreaComponent(float a_length)
			: length(a_length)
		{
		}
	};


	enum OBB_FLAG
	{
		OBB_DEFAULT = 0,
		OBB_FIXED_PITCH = 1 << 0,
		OBB_FIXED_YAW = 1 << 1,
		OBB_FIXED_ROLL = 1 << 2,
		OBB_TRIGGER = 1 << 3,
		OBB_PushOutLocked = 1 << 4
	};
	struct OBBCollider
	{
		static constexpr TypeID kTypeId = 29;
		static constexpr const char* kTypeName = "OBBCollider";
		static constexpr int kVersion = 0;

		// ・ｽ・ｽ・ｽ[・ｽ・ｽ・ｽh・ｽﾌ抵ｿｽ・ｽS・ｽ・ｽ・ｽW
		float3 center;

		// x,y,z・ｽﾌ包ｿｽ・ｽ・ｽ・ｽx・ｽN・ｽg・ｽ・ｽ
		std::array<float3, 3> axis;

		std::array<float3, 3> edges;

		BitFlag obbBitFlag;

		// ・ｽ・ｽ・ｽa	
		float3 half;

		OBBCollider(BitFlag a_flag = OBB_DEFAULT)
			: center(0.0f, 0.0f, 0.0f)
			, half(0.0f, 0.0f, 0.0f)
			, obbBitFlag(a_flag)
		{
			for (int i = 0; i < 3; i++)
			{
				axis[i] = float3();
			}

			for (int i = 0; i < 3; i++)
			{
				edges[i] = float3();
			}
		}
	};

	enum TRACKING_FLAG
	{
		TRACK_DEFAULT = 0,
		TRACK_FIXED_X = 1 << 0,
		TRACK_FIXED_Y = 1 << 1,
		TRACK_FIXED_Z = 1 << 2,
		TRACK_NONTRACK = 1 << 3
	};
	struct TrackingWarp
	{
		static constexpr TypeID kTypeId = 30;
		static constexpr const char* kTypeName = "TrackingWarp";
		static constexpr int kVersion = 0;

		Entity targetEntity;
		float3 warpDis;
		float3 trackOffset;
		BitFlag flag;

		TrackingWarp()
			: TrackingWarp(kInvalidEntity, float3(), float3(), TRACK_DEFAULT)
		{
		}

		TrackingWarp(Entity a_targetEntity, float3 a_warpDis, float3 a_trackOffset, BitFlag a_flag)
			: targetEntity(a_targetEntity)
			, warpDis(a_warpDis)
			, trackOffset(a_trackOffset)
			, flag(a_flag)

		{
		}
	};

	enum InfoColliderType
	{
		DEFAULT,
		PUSHLOCKED,
		MAX_COLLIDER_TYPE,
		NONE_INFO
	};
	struct HitInfomation
	{
		static constexpr TypeID kTypeId = 31;
		static constexpr const char* kTypeName = "HitInfomation";
		static constexpr int kVersion = 0;

		struct HitResult
		{

			// ・ｽﾔつゑｿｽ・ｽ・ｽ・ｽ・ｽEntity
			Entity hitEntity;

			// ・ｽ・ｽ・ｽ・ｽ・ｽﾟゑｿｽ・ｽ・ｽ・ｽ・ｽ
			float3 normal;
			// ・ｽ・ｽ・ｽ・ｽ・ｽﾟゑｿｽ・ｽ・ｽ
			float depth;

			InfoColliderType otherType;

			HitResult()
				: HitResult(kInvalidEntity, float3(), 0.0f, DEFAULT)
			{
			}

			HitResult(Entity a_hitEntity, float3 a_normal, float a_depth, InfoColliderType a_otherType)
				: hitEntity(a_hitEntity)
				, normal(a_normal)
				, depth(a_depth)
				, otherType(a_otherType)
			{
			}
		};

		struct TriggerResult
		{
			// ・ｽ・ｽ・ｽ・ｽ・ｽﾄゑｿｽ・ｽ・ｽEntity
			Entity triggerEntity;

			TriggerResult()
				: TriggerResult(kInvalidEntity)
			{
			}

			TriggerResult(Entity a_triggerEntity)
				: triggerEntity(a_triggerEntity)
			{
			}
		};

		std::vector<HitResult> hitResults;
		std::vector<TriggerResult> triggerResults;

		HitInfomation()
		{
		}
	};


	struct ChunkChange
	{
		static constexpr TypeID kTypeId = 32;
		static constexpr const char* kTypeName = "ChunkChange";
		static constexpr int kVersion = 0;

		bool isWait;
		std::string chunkType;

		ChunkChange()
			: ChunkChange(false, "")
		{
		}

		ChunkChange(bool a_isWait, std::string a_chunkType)
			: isWait(a_isWait)
			, chunkType(a_chunkType)
		{
		}
	};

	struct KeyChunkChange
	{
		static constexpr TypeID kTypeId = 33;
		static constexpr const char* kTypeName = "KeyChunkChange";
		static constexpr int kVersion = 0;

		std::string key;
		float waitTime;

		KeyChunkChange()
			: KeyChunkChange("", 0.0f)
		{
		}

		KeyChunkChange(std::string a_key, float a_waitTime)
			: key(a_key)
			, waitTime(a_waitTime)
		{
		}
	};

	enum EffectType
	{
		WHITEFADE_UP,
		WHITEFADE_CLEAR,
		WHITEMINIFADE_UP,
		WHITEMINIFADE_CLEAR,
		DARKFADE_UP,
		DARKFADE_CLEAR,
		SPEEDLINEFADE,
		SHOTFLASH,
		DAMAGE_EFFECT,
		DAMAGE_DIRECTION
	};
	struct CreateEffect
	{
		static constexpr TypeID kTypeId = 34;
		static constexpr const char* kTypeName = "CreateEffect";
		static constexpr int kVersion = 0;

		EffectType type;


		float2 posOffset;
		float angleOffset;

		CreateEffect()
			: CreateEffect(WHITEFADE_UP)
		{
		}

		CreateEffect(EffectType a_type)
			: type(a_type)
			, posOffset()
			, angleOffset()
		{
		}

		CreateEffect(EffectType a_type, float2 a_posOffset, float a_angleOffset)
			: type(a_type)
			, posOffset(a_posOffset)
			, angleOffset(a_angleOffset)
		{

		}
	};


	struct ZoomComponent
	{
		static constexpr TypeID kTypeId = 35;
		static constexpr const char* kTypeName = "ZoomComponent";
		static constexpr int kVersion = 0;

		float zoom;
		float maxZoom;
		float minZoom;

		ZoomComponent()
			: ZoomComponent(0.0f)
		{
		}

		ZoomComponent(float a_zoom, float a_maxZoom = 100.0f, float a_minZoom = 1.0f)
			: zoom(a_zoom)
			, maxZoom(a_maxZoom)
			, minZoom(a_minZoom)
		{
		}
	};

	enum ZoomMoveType
	{
		ZOOMMOVE_DEFAULT,
		ZOOMMOVE_ZERORESET
	};
	struct ZoomMove
	{
		static constexpr TypeID kTypeId = 36;
		static constexpr const char* kTypeName = "ZoomMove";
		static constexpr int kVersion = 0;

		ZoomMoveType moveType;
		float zoomSpeed;
		float zoomF;

		ZoomMove()
			: ZoomMove(ZOOMMOVE_DEFAULT, 0.0f, 0.0f)
		{
		}

		ZoomMove(ZoomMoveType a_moveType, float a_zoomSpeed = 0.0f, float a_zoomF = 0.0f)
			: moveType(a_moveType)
			, zoomSpeed(a_zoomSpeed)
			, zoomF(a_zoomF)
		{
		}
	};

	struct EffectKey
	{
		static constexpr TypeID kTypeId = 37;
		static constexpr const char* kTypeName = "EffectKey";
		static constexpr int kVersion = 0;

		EffectType type;
		std::string key;
		bool isRecycling;

		EffectKey()
			: EffectKey(WHITEFADE_UP, "", false)
		{
		}

		EffectKey(EffectType a_type, std::string a_key, bool a_isRecycling = false)
			: type(a_type)
			, key(a_key)
			, isRecycling(a_isRecycling)
		{
		}
	};

	struct DelayChunkChange
	{
		static constexpr TypeID kTypeId = 38;
		static constexpr const char* kTypeName = "DelayChunkChange";
		static constexpr int kVersion = 0;

		float maxDelayTime;
		float currentCount;

		DelayChunkChange()
			: DelayChunkChange(0.0f)
		{
		}

		DelayChunkChange(float a_maxDelayTime)
			: maxDelayTime(a_maxDelayTime)
			, currentCount(a_maxDelayTime)

		{
		}
	};

	enum FadeType
	{
		FADE_UP,
		FADE_DOWN,
	};

	struct FadeUI
	{
		static constexpr TypeID kTypeId = 39;
		static constexpr const char* kTypeName = "FadeUI";
		static constexpr int kVersion = 0;

		FadeType fadeType;
		float fadeSpeed;

		FadeUI()
			: FadeUI(FADE_UP, 0.0f)
		{
		}

		FadeUI(FadeType a_fadeType, float a_fadeSpeed)
			:fadeType(a_fadeType)
			, fadeSpeed(a_fadeSpeed)
		{
		}

	};

	enum FadeChungeType
	{
		FADE_CHANGE_FLICKER
	};
	struct FadeChange
	{
		static constexpr TypeID kTypeId = 40;
		static constexpr const char* kTypeName = "FadeChange";
		static constexpr int kVersion = 0;

		FadeChungeType type;
		bool isWait;

		FadeChange()
			: FadeChange(FADE_CHANGE_FLICKER, false)
		{
		}

		FadeChange(FadeChungeType a_type, bool a_isWait = false)
			: type(a_type)
			, isWait(a_isWait)
		{
		}
	};

	struct OwnerComponent
	{
		static constexpr TypeID kTypeId = 41;
		static constexpr const char* kTypeName = "OwnerComponent";
		static constexpr int kVersion = 0;

		Entity owner; // ・ｽ・ｽ・ｽg・ｽ・ｽﾛ趣ｿｽ・ｽ・ｽ・ｽﾄゑｿｽ・ｽ・ｽEntity

		OwnerComponent()
		{
			owner = kInvalidEntity;
		}
	};

	struct BulletComponent
	{
		static constexpr TypeID kTypeId = 42;
		static constexpr const char* kTypeName = "BulletComponent";
		static constexpr int kVersion = 0;

		float damage;

		BulletComponent()
			: BulletComponent(0.0f)
		{
		}

		BulletComponent(float a_damage)
			: damage(a_damage)
		{
		}
	};

	struct Camera
	{
		static constexpr TypeID kTypeId = 43;
		static constexpr const char* kTypeName = "Camera";
		static constexpr int kVersion = 0;

		// ・ｽJ・ｽ・ｽ・ｽ・ｽ・ｽﾌ趣ｿｽﾞを増やす・ｽ鼾・ｿｽﾊコ・ｽ・ｽ・ｽ|・ｽ[・ｽl・ｽ・ｽ・ｽg・ｽﾉ包ｿｽ・ｽ・ｽ・ｽ・ｽ
		size_t cameraPriority;

		float3 lookPosition;
		float3 upVector;

		float	fovy;		// ・ｽ・ｽp
		float	aspect;	//・ｽ@・ｽA・ｽX・ｽy・ｽN・ｽg・ｽ・ｽ
		float	nearCrip;		// ・ｽj・ｽA・ｽN・ｽ・ｽ・ｽb・ｽv
		float	farCrip;		// ・ｽt・ｽ@・ｽ[・ｽN・ｽ・ｽ・ｽb・ｽv

		Camera()
			: Camera(0, float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0f), 60.0f, 1.77777f, 0.05f, 1000.0f)
		{
		}

		Camera(size_t a_cameraPrioriy, float3 a_lookPosition, float3 a_upVector, float a_fovy, float a_aspect, float a_nearCrip, float a_farCrip)
			: cameraPriority(a_cameraPrioriy)
			, lookPosition(a_lookPosition)
			, upVector(a_upVector)
			, fovy(a_fovy)
			, aspect(a_aspect)
			, nearCrip(a_nearCrip)
			, farCrip(a_farCrip)
		{
		}
	};

	struct CameraPoint
	{
		static constexpr TypeID kTypeId = 44;
		static constexpr const char* kTypeName = "CameraPoint";
		static constexpr int kVersion = 1;

		size_t cameraPointPriority;
		float posLeapSpeed;
		float rotLeapSpeed;
		float3 posOffset;
		float3 rotOffset;

		CameraPoint()
			: CameraPoint(0, 0.0f, 0.0f)
		{
		}

		CameraPoint(size_t a_cameraPointPriority, float a_posLeapSpeed, float a_rotLeapSpeed, float3 a_posOffset = float3(), float3 a_rotOffset = float3())
			: cameraPointPriority(a_cameraPointPriority)
			, posLeapSpeed(a_posLeapSpeed)
			, rotLeapSpeed(a_rotLeapSpeed)
			, posOffset(a_posOffset)
			, rotOffset(a_rotOffset)
		{
		}
	};

	enum FollowPosFlag
	{
		FOLLOW_POS_DEFAULT = 0,
		FOLLOW_POS_FIXED_X = 1 << 0,
		FOLLOW_POS_FIXED_Y = 1 << 1,
		FOLLOW_POS_FIXED_Z = 1 << 2,
		FOLLOW_POS_LOCALOFFSET = 1 << 3
	};

	struct FollowPosition
	{
		static constexpr TypeID kTypeId = 45;
		static constexpr const char* kTypeName = "FollowPosition";
		static constexpr int kVersion = 0;

		Entity targetEntity;
		float3 positionOffset;
		BitFlag flag;


		FollowPosition()
			: FollowPosition(float3(), kInvalidEntity, FOLLOW_POS_DEFAULT)
		{
		}

		FollowPosition(float3 a_positionOffset, Entity a_targetEntity, BitFlag a_flag = FOLLOW_POS_DEFAULT)
			: positionOffset(a_positionOffset)
			, targetEntity(a_targetEntity)
			, flag(a_flag)
		{
		}

		FollowPosition(float x, float y, float z, Entity a_targetEntity, BitFlag a_flag = FOLLOW_POS_DEFAULT)
			: positionOffset(float3(x, y, z))
			, targetEntity(a_targetEntity)
			, flag(a_flag)
		{
		}
	};

	enum FollowRotFlag
	{
		FOLLOW_ROT_DEFAULT,
		FOLLOW_ROT_FIXED_X,
		FOLLOW_ROT_FIXED_Y,
		FOLLOW_ROT_FIXED_Z
	};

	struct FollowRotation
	{
		static constexpr TypeID kTypeId = 46;
		static constexpr const char* kTypeName = "FollowRotation";
		static constexpr int kVersion = 0;

		Entity targetEntity;
		float3 rotationOffset;
		BitFlag flag;

		FollowRotation()
			: FollowRotation(float3(), kInvalidEntity, FOLLOW_ROT_DEFAULT)
		{
		}

		FollowRotation(float3 a_rotationOffset, Entity a_targetEntity, BitFlag a_flag = FOLLOW_ROT_DEFAULT)
			: rotationOffset(a_rotationOffset)
			, targetEntity(a_targetEntity)
			, flag(a_flag)
		{
		}

		FollowRotation(float x, float y, float z, Entity a_targetEntity, BitFlag a_flag = FOLLOW_ROT_DEFAULT)
			: rotationOffset(float3(x, y, z))
			, targetEntity(a_targetEntity)
			, flag(a_flag)
		{
		}
	};

	struct Ray
	{
		static constexpr TypeID kTypeId = 47;
		static constexpr const char* kTypeName = "Ray";
		static constexpr int kVersion = 0;

		float3 rayOffset;

		Ray()
			: Ray(float3())
		{
		}

		Ray(float3 a_rayOffset)
			: rayOffset(a_rayOffset)
		{
		}

		Ray(float x, float y, float z)
			: rayOffset({ x, y, z })
		{
		}
	};

	struct RayInfomation
	{
		static constexpr TypeID kTypeId = 48;
		static constexpr const char* kTypeName = "RayInfomation";
		static constexpr int kVersion = 0;

		Entity rayOwner;
		Entity otherEntity;

		float3 rayHitPosition;
		float3 rayHitRotation;

		RayInfomation()
			: RayInfomation(kInvalidEntity, kInvalidEntity, float3(), float3())
		{
		}

		RayInfomation(Entity a_rayOwner, Entity a_otherEntity, float3 a_rayHitPosition, float3 a_rayHitRotation)
			: rayOwner(a_rayOwner)
			, otherEntity(a_otherEntity)
			, rayHitPosition(a_rayHitPosition)
			, rayHitRotation(a_rayHitRotation)
		{
		}
	};

	struct LifeTime
	{
		static constexpr TypeID kTypeId = 49;
		static constexpr const char* kTypeName = "LifeTime";
		static constexpr int kVersion = 0;

		float time;

		LifeTime()
			: LifeTime(0.0f)
		{
		}

		LifeTime(float a_time)
			: time(a_time)
		{
		}
	};


	enum ModelDrawType
	{
		MODEL_DRAW_DEFAULT = 0,
		MODEL_DRAW_SHADOW = 1 << 0,
		MODEL_DRAW_NOTDEPTH = 1 << 1,
		MODEL_DRAW_LOCAL_OFFSET = 1 << 2,
	};

	struct TrailComponent
	{
		static constexpr TypeID kTypeId = 50;
		static constexpr const char* kTypeName = "TrailComponent";
		static constexpr int kVersion = 0;

		TrailEffect::LineID id;
		float shrinkageRate;
		float size;

		TrailComponent()
			: TrailComponent(static_cast<TrailEffect::LineID>(0), 0.0f, 0.0f)
		{
		}

		TrailComponent(TrailEffect::LineID a_id, float a_shrinkageRate, float a_size)
			: id(a_id)
			, shrinkageRate(a_shrinkageRate)
			, size(a_size)
		{
		}
	};

	struct ModelKey
	{
		static constexpr TypeID kTypeId = 51;
		static constexpr const char* kTypeName = "ModelKey";
		static constexpr int kVersion = 0;

		std::string key;
		float3 offset;
		float3 rotato;
		float3 defaultRotato;
		bool useAnime;
		BitFlag modelDrawFlag;

		ModelKey()
			: ModelKey("")
		{
		}

		ModelKey(std::string a_key, float3 a_offset = float3(), float3 a_rotato = float3(), BitFlag a_modelDrawFlag = MODEL_DRAW_DEFAULT, bool a_useAnime = false)
			: key(a_key)
			, offset(a_offset)
			, defaultRotato(a_rotato)
			, rotato(a_rotato)
			, modelDrawFlag(a_modelDrawFlag)
			, useAnime(a_useAnime)
		{
		}
	};

	struct UIComponent
	{
		static constexpr TypeID kTypeId = 52;
		static constexpr const char* kTypeName = "UIComponent";
		static constexpr int kVersion = 2;

		std::string key;
		float2 uiPos;
		float2 uiScale;
		float uiRotation;
		float alpha;
		float2 uvPos;
		float2 uvScale;
		bool isActiv;

		UIComponent()
			: UIComponent("", float2(), float2(), 0.0f, 1.0f)
		{
		}

		UIComponent(std::string a_key, float2 a_pos, float2 a_scale, float a_rotation, float a_alpha = 1.0f, float2 a_uvPos = { 0.0f, 0.0f }, float2 a_uvScale = { 1.0f, 1.0f }, bool a_isActiv = true)
			: key(a_key)
			, uiPos(a_pos)
			, uiScale(a_scale)
			, uiRotation(a_rotation)
			, alpha(a_alpha)
			, uvPos(a_uvPos)
			, uvScale(a_uvScale)
			, isActiv(a_isActiv)
		{
		}
	};

	// TODO Json・ｽﾇみ搾ｿｽ・ｽﾝゑｿｽ・ｽX・ｽV・ｽ・ｽ・ｽ・ｽ
	struct SpriteComponent
	{
		static constexpr TypeID kTypeId = 53;
		static constexpr const char* kTypeName = "SpriteComponent";
		static constexpr int kVersion = 2;

		float3 offsetPos;
		float3 offsetRotation;
		bool isBillBoard;

		SpriteComponent()
			: SpriteComponent(float3(), float3(), false)
		{
		}

		SpriteComponent(float3 a_offsetPos, float3 a_offsetRotation, bool a_isBillBoard = false)
			: offsetPos(a_offsetPos)
			, offsetRotation(a_offsetRotation)
			, isBillBoard(a_isBillBoard)
		{
		}
	};


	struct Poliline
	{
		static constexpr TypeID kTypeId = 54;
		static constexpr const char* kTypeName = "Poliline";
		static constexpr int kVersion = 0;
		Poliline() = default;
	};

	struct LeapPosComponent
	{
		static constexpr TypeID kTypeId = 55;
		static constexpr const char* kTypeName = "LeapPosComponent";
		static constexpr int kVersion = 0;

		bool isActicv;
		float3 leapStartPos;
		float3 leapEndPos;
		float leapTime;
		float leapMaxTime;
		float leapRate;

		LeapPosComponent()
			: LeapPosComponent(false, float3(), float3(), 0.0f)
		{
		}

		LeapPosComponent(
			bool a_isActiv,
			float3 a_leapStartPos,
			float3 a_leapEndPos,
			float a_leapMaxTime)
			: isActicv(a_isActiv)
			, leapStartPos(a_leapStartPos)
			, leapEndPos(a_leapEndPos)
			, leapTime(0.0f)
			, leapMaxTime(a_leapMaxTime)
			, leapRate(0.0f)
		{
		}
	};

	struct LeapRotComponent
	{
		static constexpr TypeID kTypeId = 56;
		static constexpr const char* kTypeName = "LeapRotComponent";
		static constexpr int kVersion = 0;

		bool isActicv;
		float3 leapStartRot;
		float3 leapEndRot;
		float leapTime;
		float leapMaxTime;
		float leapRate;

		LeapRotComponent()
			: LeapRotComponent(false, float3(), float3(), 0.0f)
		{
		}

		LeapRotComponent(
			bool a_isActiv,
			float3 a_leapStartRot,
			float3 a_leapEndRot,
			float a_leapMaxTime)
			: isActicv(a_isActiv)
			, leapStartRot(a_leapStartRot)
			, leapEndRot(a_leapEndRot)
			, leapTime(0.0f)
			, leapMaxTime(a_leapMaxTime)
			, leapRate(0.0f)
		{
		}
	};

	struct FlipComponent
	{
		static constexpr TypeID kTypeId = 57;
		static constexpr const char* kTypeName = "LeapRotComponent";
		static constexpr int kVersion = 0;
		FlipComponent() = default;
	};

	struct UIAngularSpeed
	{
		static constexpr TypeID kTypeId = 58;
		static constexpr const char* kTypeName = "UIAngularSpeed";
		static constexpr int kVersion = 0;

		float speed;

		UIAngularSpeed()
			: UIAngularSpeed(0.0f)
		{
		}

		UIAngularSpeed(float a_speed)
			: speed(a_speed)
		{
		}
	};


	struct Pose
	{
		static constexpr TypeID kTypeId = 71;
		static constexpr const char* kTypeName = "Pose";
		static constexpr int kVersion = 0;

		float3 pos;
		float3 rot;

		Pose()
			: pos()
			, rot()
		{
		}
	};

	enum PosePosStateEnum
	{
		POSE_POS_NONE,
		POSE_POS_RAIL,
		POSE_POS_CAMERA,
		POSE_POS_FOLLOW,
		POSE_POS_LEAP,
		POSE_POS_DEBUGCAMERA,
	};

	struct PosePosState
	{
		static constexpr TypeID kTypeId = 72;
		static constexpr const char* kTypeName = "PosePosState";
		static constexpr int kVersion = 0;

		PosePosStateEnum state;

		PosePosState(PosePosStateEnum a_state)
			: state(a_state)
		{
		}

		PosePosState()
			: PosePosState(POSE_POS_NONE)
		{
		}
	};

	enum PoseRotStateEnum
	{
		POSE_ROT_NONE,
		POSE_ROT_LOOK,
		POSE_ROT_CAMERA,
		POSE_ROT_FOLLOW,
		POSE_ROT_DEBUGCAMERA,
		POSE_ROT_LOOKMOVE
	};

	struct PoseRotState
	{
		static constexpr TypeID kTypeId = 73;
		static constexpr const char* kTypeName = "PoseRotState";
		static constexpr int kVersion = 0;

		PoseRotStateEnum state;

		PoseRotState(PoseRotStateEnum a_state)
			: state(a_state)
		{
		}

		PoseRotState()
			: PoseRotState(POSE_ROT_NONE)
		{
		}
	};


	struct RailComponent
	{
		static constexpr TypeID kTypeId = 76;
		static constexpr const char* kTypeName = "RailComponent";
		static constexpr int kVersion = 0;

		bool isActicv;
		float3 railStartPos;
		float3 railEndPos;
		float3 currentPos;
		float railTime;
		float railMaxTime;
		float railRate;

		RailComponent()
			: RailComponent(false, float3(), float3(), 0.0f)
		{
		}

		RailComponent(
			bool a_isActiv,
			float3 a_railStartPos,
			float3 a_railEndPos,
			float a_railMaxTime)
			: isActicv(a_isActiv)
			, railStartPos(a_railStartPos)
			, railEndPos(a_railEndPos)
			, currentPos(a_railStartPos)
			, railTime(0.0f)
			, railMaxTime(a_railMaxTime)
			, railRate(0.0f)
		{
		}
	};

	struct RailUser
	{
		static constexpr TypeID kTypeId = 77;
		static constexpr const char* kTypeName = "RailUser";
		static constexpr int kVersion = 0;

		Entity targetRail;

		RailUser(Entity a_targetRail)
			: targetRail(a_targetRail)
		{
		}

		RailUser()
			: RailUser(kInvalidEntity)
		{
		}
	};

	struct RailFly
	{
		static constexpr TypeID kTypeId = 79;
		static constexpr const char* kTypeName = "RailFly";
		static constexpr int kVersion = 0;

		float flyPow;
		float flyProgress;
		float flySpeed;

		RailFly()
			: RailFly(0.0f, 0.0f)
		{
		}

		RailFly(float a_flyPow, float a_flySpeed)
			: flyPow(a_flyPow)
			, flyProgress(0.0f)
			, flySpeed(a_flySpeed)
		{
		}
	};


	struct AngleLimitComponent
	{
		static constexpr TypeID kTypeId = 83;
		static constexpr const char* kTypeName = "AngleLimitComponent";
		static constexpr int kVersion = 1;

		bool isLimitX;
		bool isLimitY;

		float2 maxAngle;

		float2 minAngle;

		AngleLimitComponent()
			: AngleLimitComponent(false, false, 0.0f, 0.0f, 0.0f, 0.0f)
		{
		}

		AngleLimitComponent(bool a_isLimitX, bool a_isLimitY, float a_maxX, float a_minX, float a_maxY, float a_minY)
			: isLimitX(a_isLimitX)
			, isLimitY(a_isLimitY)
			, maxAngle(a_maxX, a_maxY)
			, minAngle(a_minX, a_minY)
		{
		}
	};

	struct DebugCameraTag
	{
		static constexpr TypeID kTypeId = 84;
		static constexpr const char* kTypeName = "DebugCameraTag";
		static constexpr int kVersion = 0;

		DebugCameraTag() {};
	};

	struct DebugInputPos
	{
		static constexpr TypeID kTypeId = 87;
		static constexpr const char* kTypeName = "DebugInputPos";
		static constexpr int kVersion = 0;

		float2 moveSpeed;
		bool isInput;

		DebugInputPos()
			: DebugInputPos(float2())
		{
		}

		DebugInputPos(float2 a_moveSpeed)
			:moveSpeed(a_moveSpeed)
			, isInput(false)
		{
		}
	};

	struct DebugInputRot
	{
		static constexpr TypeID kTypeId = 88;
		static constexpr const char* kTypeName = "DebugInputRot";
		static constexpr int kVersion = 0;

		float2 rotatoSpeed;

		DebugInputRot()
			: DebugInputRot(float2())
		{
		}

		DebugInputRot(float2 a_rotatoSpeed)
			: rotatoSpeed(a_rotatoSpeed)
		{
		}
	};

	
	struct EnemyShooter
	{
		static constexpr TypeID kTypeId = 90;
		static constexpr const char* kTypeName = "EnemyShooter";
		static constexpr int kVersion = 0;
		float shotTimer;
		float maxShotWaitTime;

		EnemyShooter(float a_maxShotWaitTimer)
			: shotTimer(a_maxShotWaitTimer)
			, maxShotWaitTime(a_maxShotWaitTimer)
		{
		}

		EnemyShooter()
			: shotTimer(0.0f)
			, maxShotWaitTime(0.0f)
		{

		}
	};

	struct EnemyBulletTag
	{
		static constexpr TypeID kTypeId = 91;
		static constexpr const char* kTypeName = "EnemyBulletTag";
		static constexpr int kVersion = 0;
		EnemyBulletTag() {};
	};

	struct HitPoint
	{
		static constexpr TypeID kTypeId = 92;
		static constexpr const char* kTypeName = "HitPoint";
		static constexpr int kVersion = 0;
		float maxHP;
		float currentHP;

		HitPoint(float a_maxHP)
			: maxHP(a_maxHP)
			, currentHP(a_maxHP)
		{
		}

		HitPoint(float a_maxHP, float a_currentHP)
			: maxHP(a_maxHP)
			, currentHP(a_currentHP)
		{
		}

		HitPoint()
			: HitPoint(0.0f)
		{
		}
	};

	struct DeadState
	{
		static constexpr TypeID kTypeId = 93;
		static constexpr const char* kTypeName = "DeadState";
		static constexpr int kVersion = 0;
		bool isDead;

		DeadState(bool a_isDead = false)
			: isDead(a_isDead)
		{
		}

	};

	struct HPGaugeUI
	{
		static constexpr TypeID kTypeId = 94;
		static constexpr const char* kTypeName = "HPGaugeUI";
		static constexpr int kVersion = 0;
		Entity target;
		float2 gaugePos;
		float2 gaugeScale;

		HPGaugeUI()
			: target(kInvalidEntity)
			, gaugePos()
			, gaugeScale()
		{

		}

		HPGaugeUI(Entity a_target, float2 a_gaugePos, float2 a_gaugeScale)
			: target(a_target)
			, gaugePos(a_gaugePos)
			, gaugeScale(a_gaugeScale)
		{
		}

	};

	struct ShakeComponent
	{
		static constexpr TypeID kTypeId = 95;
		static constexpr const char* kTypeName = "ShakeComponent";
		static constexpr int kVersion = 0;

		// ・ｽU・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ
		float shakeTime;
		// ・ｽU・ｽ・ｽ・ｽﾌ大き・ｽ・ｽ
		float3 shakePower;
		//! ・ｽU・ｽ・ｽ・ｽﾌ鯉ｿｽ・ｽ・ｽ・ｽ・ｽ
		float3 shakeAmplitude;
		//! ・ｽo・ｽﾟ趣ｿｽ・ｽ・ｽ
		float elapsedTime;

		ShakeComponent(float3 a_shakePower, float3 a_shakeAmplitude, float shakeTime = 0.0f)
			: shakePower(a_shakePower)
			, shakeAmplitude(a_shakeAmplitude)
			, shakeTime(shakeTime)
			, elapsedTime(0.0f)
		{
		}

		ShakeComponent() : ShakeComponent(float3(1.0f, 1.0f, 1.0f), float3(1.0f, 1.0f, 1.0f), 0.0f)
		{
		}
	};


	struct AddDamageComponent
	{
		static constexpr TypeID kTypeId = 97;
		static constexpr const char* kTypeName = "AddDamageComponent";
		static constexpr int kVersion = 0;

		float damageValue;

		AddDamageComponent(float a_damageValue)
			: damageValue(a_damageValue)
		{
		}

		AddDamageComponent()
			: AddDamageComponent(0.0f)
		{
		}

	};

	struct SpriteAnimation
	{
		static constexpr TypeID kTypeId = 98;
		static constexpr const char* kTypeName = "SpriteAnimation";
		static constexpr int kVersion = 0;

		int maxSprite;
		int nextSprite;
		int2 spriteUVCount;
		bool isLoop;

		SpriteAnimation(int a_maxSprite, int2 a_spriteUVCount, bool a_isLoop, int a_startSprite = 0)
			: spriteUVCount(a_spriteUVCount)
			, maxSprite(a_maxSprite)
			, isLoop(a_isLoop)
			, nextSprite(a_startSprite)
		{
		}

		SpriteAnimation()
			: SpriteAnimation(1, int2(1, 1), false, 0)
		{
		}
	};


	enum ShotUIType
	{
		SHOTUI_NONE = 0,
		SHOTUI_CANSHOT = 1 << 0, // ・ｽﾋ鯉ｿｽ・ｽﾂ能・ｽ・ｽ・ｽﾉ描・ｽ謔ｳ・ｽ・ｽ・ｽUI
		SHOTUI_WAIT = 1 << 1, // ・ｽﾋ鯉ｿｽ・ｽﾒ機・ｽ・ｽ・ｽﾉ描・ｽ謔ｳ・ｽ・ｽ・ｽUI
		SHOTUI_FADERESET_UP = 1 << 2, // ・ｽﾋ鯉ｿｽ・ｽ?ｵ変更・ｽ・ｽ・ｽﾉ難ｿｽ・ｽ・ｽ・ｽx・ｽﾈどゑｿｽ・ｽ・ｽ・ｽZ・ｽb・ｽg・ｽ・ｽ・ｽ・ｽ・ｽ
		SHOTUI_FADERESET_DOWN = 1 << 3
	};
	struct ShotUI
	{
		static constexpr TypeID kTypeId = 99;
		static constexpr const char* kTypeName = "ShotUI";
		static constexpr int kVersion = 0;

		BitFlag uiType;

		ShotUI(BitFlag a_uiType)
			: uiType(a_uiType)
		{
		}

		ShotUI()
			: ShotUI(SHOTUI_NONE)
		{
		}
	};

	struct UVMove
	{
		static constexpr TypeID kTypeId = 100;
		static constexpr const char* kTypeName = "UVMove";
		static constexpr int kVersion = 0;

		float2 moveSpeed;

		UVMove(float2 a_moveSpeed)
			: moveSpeed(a_moveSpeed)
		{
		}

		UVMove()
			: UVMove(float2(0.0f, 0.0f))
		{
		}
	};

	// ・ｽ・ｽ・ｽt・ｽ・ｽ・ｽ[・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽp・ｽ・ｽ・ｽ・ｽ・ｽR・ｽﾚ難ｿｽ
	struct MotionTransform
	{
		static constexpr TypeID kTypeId = 101;
		static constexpr const char* kTypeName = "MotionTransform";
		static constexpr int kVersion = 0;

		float3 motionPos;
		float3 motionRot;

		MotionTransform(float3 a_position, float3 a_rotation)
			: motionPos(a_position)
			, motionRot(a_rotation)
		{
		}

		MotionTransform()
			: MotionTransform(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f))
		{
		}
	};


	// ・ｽﾅ抵ｿｽ・ｽ・ｽW・ｽ・ｽResult
	struct FixedResult
	{
		static constexpr TypeID kTypeId = 102;
		static constexpr const char* kTypeName = "FixedResult";
		static constexpr int kVersion = 0;

		float3 newPos;
		float3 newRot;

		FixedResult()
			: newPos(0.0f, 0.0f, 0.0f)
			, newRot(0.0f, 0.0f, 0.0f)
		{
		}
	};

	// ・ｽ・ｽ・ｽR・ｽﾚ難ｿｽ・ｽ・ｽResult
	struct MotionResult
	{
		static constexpr TypeID kTypeId = 103;
		static constexpr const char* kTypeName = "MotionResult";
		static constexpr int kVersion = 0;

		float3 posOffset;
		float3 rotOffset;

		bool isWarp;
		float3 warpPos;

		MotionResult()
			: posOffset(0.0f, 0.0f, 0.0f)
			, rotOffset(0.0f, 0.0f, 0.0f)
			, isWarp(false)
			, warpPos(0.0f, 0.0f, 0.0f)
		{
		}
	};

	// 1・ｽt・ｽ・ｽ・ｽ[・ｽ・ｽ・ｽ・ｽ・ｽ・ｽﾌ移難ｿｽResult
	struct EphemeralResult
	{
		static constexpr TypeID kTypeId = 104;
		static constexpr const char* kTypeName = "EphemeralResult";
		static constexpr int kVersion = 0;

		float3 posOffset;
		float3 rotOffset;

		EphemeralResult()
			: posOffset(0.0f, 0.0f, 0.0f)
			, rotOffset(0.0f, 0.0f, 0.0f)
		{
		}
	};

	enum FireworkType
	{
		FIREWORK_NONE = 0,
		FIREWORK_BURN = 1 << 0,
		FIREWORK_DROP = 1 << 1,
		FIREWORK_FLY = 1 << 2,
		FIREWORK_SHAKE = 1 << 3,
		FIREWORK_RETRO = 1 << 4,
		FIREWORK_INPUT_MOVE = 1 << 5,
		FIREWORK_INPUT_ANGLE = 1 << 6,
		FIREWORK_FISH = 1 << 7

	};

	struct Firework
	{
		static constexpr TypeID kTypeId = 105;
		static constexpr const char* kTypeName = "Firework";
		static constexpr int kVersion = 0;

		BitFlag fireworkType;

		Firework(BitFlag flag)
		{
			fireworkType = flag;
		}

		Firework()
			: fireworkType(FIREWORK_NONE)
		{

		}
	};

	struct AlphaBlendComponent
	{
		static constexpr TypeID kTypeId = 106;
		static constexpr const char* kTypeName = "AlphaBlendComponent";
		static constexpr int kVersion = 0;

		std::string textureKey1;
		std::string textureKey2;

		AlphaBlendComponent(std::string a_textureKey1, std::string a_textureKey2)
			: textureKey1(a_textureKey1)
			, textureKey2(a_textureKey2)
		{
		}

		AlphaBlendComponent()
			:AlphaBlendComponent("", "")
		{
		}
	};


	struct AngularVelocity
	{
		static constexpr TypeID kTypeId = 107;
		static constexpr const char* kTypeName = "AngularVelocity";
		static constexpr int kVersion = 0;

		float3 velocity;
		AngularVelocity(float3 a_velocity)
			: velocity(a_velocity)
		{
		}

		AngularVelocity()
			: AngularVelocity(float3(0.0f, 0.0f, 0.0f))
		{
		}
	};

	struct SectorHitJudge
	{
		static constexpr TypeID kTypeId = 108;
		static constexpr const char* kTypeName = "SectorHitJudge";
		static constexpr int kVersion = 0;

		// ・ｽ・ｽﾌ範茨ｿｽ
		float minLength;
		float maxLength;
		// ・ｽ・ｽﾌ角・ｽx
		float angle;
		// ・ｽ繪ｺ・ｽ・ｽ・ｽ・ｽ・ｽﾖの費ｿｽ・ｽ・ｽ
		float maxHeight;
		float maxLowness;

		SectorHitJudge(float a_minLength, float a_maxLength, float a_angle, float a_maxHeight, float a_maxLowness)
			: minLength(a_minLength)
			, maxLength(a_maxLength)
			, angle(a_angle)
			, maxHeight(a_maxHeight)
			, maxLowness(a_maxLowness)
		{
		}

		SectorHitJudge()
			: SectorHitJudge(0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
		{
		}
	};

	struct PlayerAttackTag
	{
		static constexpr TypeID kTypeId = 109;
		static constexpr const char* kTypeName = "PlayerAttackTag";
		static constexpr int kVersion = 0;
		PlayerAttackTag() {};
	};

	// !!!New!!!
	enum ActionFlag : BitFlag
	{
		ActionFlag_Move = 1 << 0,
		ActionFlag_Aim = 1 << 1,
		ActionFlag_Attack = 1 << 2,
		ActionFlag_Jump = 1 << 3,
		ActionFlag_Guard = 1 << 4,
		ActionFlag_KnockBack = 1 << 5,
		ActionFlag_LookMove = 1 << 6,
		ActionFlag_RotChange = 1 << 7,
		ActionFlag_All = ActionFlag_Move | ActionFlag_Aim | ActionFlag_Attack | ActionFlag_Jump | ActionFlag_Guard | ActionFlag_KnockBack | ActionFlag_LookMove | ActionFlag_RotChange,
	};


	struct AttackHitRecord
	{
		static constexpr TypeID kTypeId = 110;
		static constexpr const char* kTypeName = "AttackHitRecord";
		static constexpr int kVersion = 0;

		std::unordered_map<Entity, float> entries;

		AttackHitRecord()
		{
		}
	};


	// !!!New!!!
	struct GroundedState
	{
		static constexpr TypeID kTypeId = 111;
		static constexpr const char* kTypeName = "GroundedState";
		static constexpr int kVersion = 0;

		bool isGrounded;

		GroundedState(bool a_isGrounded = false)
			: isGrounded(a_isGrounded)
		{
		}
	};

	// !!!New!!!
	struct ActionMask
	{
		static constexpr TypeID kTypeId = 112;
		static constexpr const char* kTypeName = "ActionMask";
		static constexpr int kVersion = 0;

		BitFlag allowed;

		ActionMask(BitFlag a_allowed = ActionFlag_All)
			: allowed(a_allowed)
		{
		}
	};

	// !!!New!!!
	struct JumpAction
	{
		static constexpr TypeID kTypeId = 113;
		static constexpr const char* kTypeName = "JumpAction";
		static constexpr int kVersion = 0;

		JumpAction() = default;
	};

	struct WorldPower
	{
		static constexpr TypeID kTypeId = 114;
		static constexpr const char* kTypeName = "WorldPower";
		static constexpr int kVersion = 0;

		float3 power;
		float3 rate;

		WorldPower()
			: WorldPower(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f))
		{
		}

		WorldPower(float3 a_power, float3 a_rate)
			: power(a_power)
			, rate(a_rate)
		{
		}
	};

	struct JumpPower
	{
		static constexpr TypeID kTypeId = 115;
		static constexpr const char* kTypeName = "JumpPower";
		static constexpr int kVersion = 0;

		float3 initialVelocity;

		JumpPower()
			: JumpPower(float3(0.0f, 0.0f, 0.0f))
		{
		}

		JumpPower(float3 a_initialVelocity)
			: initialVelocity(a_initialVelocity)
		{
		}
	};

	// !!!New!!!
	// 技1発分の定義構造体
	struct AttackPower
	{
		float minLength;
		float maxLength;
		float angle;
		float maxHeight;
		float maxLowness;
		float damageValue;
		float motionTime;
		float lifeTime;
		float3 followOffset;
		float waitTime;
		// !!!New!!!
		float startupTime;
		// !!!New!!!
		std::string sectorKey;


		AttackPower()
			: AttackPower(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, float3(), 0.0f, 0.0f, "")
		{
		}

		AttackPower(
			float a_minLength,
			float a_maxLength,
			float a_angle,
			float a_maxHeight,
			float a_maxLowness,
			float a_damageValue,
			float a_motionTime,
			float a_lifeTime,
			float3 a_followOffset = float3(),
			float a_waitTime = 0.0f,
			float a_startupTime = 0.0f,
			std::string a_sectorKey = ""
			)
			: minLength(a_minLength)
			, maxLength(a_maxLength)
			, angle(a_angle)
			, maxHeight(a_maxHeight)
			, maxLowness(a_maxLowness)
			, damageValue(a_damageValue)
			, motionTime(a_motionTime)
			, lifeTime(a_lifeTime)
			, followOffset(a_followOffset)
			, waitTime(a_waitTime)
			, startupTime(a_startupTime)
			, sectorKey(std::move(a_sectorKey))
		{
		}
	};

	// 攻撃情報を実装
	struct AttackStatus
	{
		static constexpr TypeID kTypeId = 116;
		static constexpr const char* kTypeName = "AttackStatus";
		static constexpr int kVersion = 0;

		std::vector<AttackPower> attackPowers;

		AttackStatus()
		{
		}

		explicit AttackStatus(std::vector<AttackPower> a_attackPowers)
			: attackPowers(std::move(a_attackPowers))
		{
		}
	};

	struct AttackAction
	{
		static constexpr TypeID kTypeId = 117;
		static constexpr const char* kTypeName = "AttackAction";
		static constexpr int kVersion = 0;

		Entity attackEntity;
		float elapsedTime;
		int attackIndex;
		float maxDuration;

		AttackAction()
			: AttackAction(kInvalidEntity, 0.0f, 0, 0.0f)
		{
		}

		AttackAction(Entity a_attackEntity, float a_elapsedTime = 0.0f)
			: AttackAction(a_attackEntity, a_elapsedTime, 0, 0.0f)
		{
		}

		// !!!New!!!
		AttackAction(Entity a_attackEntity, float a_elapsedTime, int a_attackIndex, float a_maxDuration)
			: attackEntity(a_attackEntity)
			, elapsedTime(a_elapsedTime)
			, attackIndex(a_attackIndex)
			, maxDuration(a_maxDuration)
		{
		}
	};

	// !!!New!!!
	struct AttackWaitAction
	{
		static constexpr TypeID kTypeId = 118;
		static constexpr const char* kTypeName = "AttackWaitAction";
		static constexpr int kVersion = 0;

		float elapsedTime;
		// !!!New!!!
		float waitDuration;

		AttackWaitAction(float a_elapsedTime = 0.0f, float a_waitDuration = 0.0f)
			: elapsedTime(a_elapsedTime)
			, waitDuration(a_waitDuration)
		{
		}
	};

	// !!!New!!!
	struct LookMove
	{
		static constexpr TypeID kTypeId = 119;
		static constexpr const char* kTypeName = "LookMove";
		static constexpr int kVersion = 0;

		float rotateSpeed;

		LookMove(float a_rotateSpeed = 0.0f)
			: rotateSpeed(a_rotateSpeed)
		{
		}
	};

	// !!!New!!!
	enum class InputOrigin
	{
		None = 0,
		Device = 1,
		AI = 2,
	};

	// !!!New!!!
	struct MoveInputResult
	{
		static constexpr TypeID kTypeId = 120;
		static constexpr const char* kTypeName = "MoveInputResult";
		static constexpr int kVersion = 0;

		float2 moveDir;
		float magnitube;
		bool isInput;
		// 攻撃アクション
		bool useAttack;
		int attackIndex;

		// 防御アクション
		bool useJump;
		bool useGuard;

		MoveInputResult()
			: moveDir()
			, magnitube(0.0f)
			, isInput(false)
			, useAttack(false)
			, attackIndex(0)
			, useJump(false)
			, useGuard(false)
		{
		}
	};

	// !!!New!!!
	struct InputSource
	{
		static constexpr TypeID kTypeId = 121;
		static constexpr const char* kTypeName = "InputSource";
		static constexpr int kVersion = 0;

		InputOrigin move;
		InputOrigin attack;
		InputOrigin jump;
		InputOrigin guard;

		InputSource(InputOrigin a_origin = InputOrigin::Device)
			: move(a_origin)
			, attack(a_origin)
			, jump(a_origin)
			, guard(a_origin)
		{
		}

		InputSource(InputOrigin a_move, InputOrigin a_attack, InputOrigin a_jump, InputOrigin a_guard)
			: move(a_move)
			, attack(a_attack)
			, jump(a_jump)
			, guard(a_guard)
		{
		}
	};

	
	// !!!New!!!
	struct EnemyAttackTag
	{
		static constexpr TypeID kTypeId = 122;
		static constexpr const char* kTypeName = "EnemyAttackTag";
		static constexpr int kVersion = 0;
		EnemyAttackTag() {};
	};

	// 攻撃前隙の表示用扇（判定は持たない。形状は SectorHitJudge と同型）
	struct AttackTelegraph
	{
		static constexpr TypeID kTypeId = 123;
		static constexpr const char* kTypeName = "AttackTelegraph";
		static constexpr int kVersion = 0;

		float minLength;
		float maxLength;
		float angle;
		float maxHeight;
		float maxLowness;
		// !!!New!!!
		std::string sectorKey;

		AttackTelegraph(
			float a_minLength,
			float a_maxLength,
			float a_angle,
			float a_maxHeight,
			float a_maxLowness,
			std::string a_sectorKey = "")
			: minLength(a_minLength)
			, maxLength(a_maxLength)
			, angle(a_angle)
			, maxHeight(a_maxHeight)
			, maxLowness(a_maxLowness)
			, sectorKey(std::move(a_sectorKey))
		{
		}

		AttackTelegraph()
			: AttackTelegraph(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, "")
		{
		}
	};

	// 攻撃前隙アクション
	struct AttackStartupAction
	{
		static constexpr TypeID kTypeId = 124;
		static constexpr const char* kTypeName = "AttackStartupAction";
		static constexpr int kVersion = 0;

		float elapsedTime;
		float startupDuration;
		int attackIndex;
		// 削除できるよう判定表示Entityを保持
		Entity telegraphEntity;

		AttackStartupAction()
			: AttackStartupAction(0.0f, 0, kInvalidEntity)
		{
		}

		AttackStartupAction(
			float a_startupDuration,
			int a_attackIndex,
			Entity a_telegraphEntity)
			: elapsedTime(0.0f)
			, startupDuration(a_startupDuration)
			, attackIndex(a_attackIndex)
			, telegraphEntity(a_telegraphEntity)
		{
		}
	};

	// !!!New!!!
	struct AttackInstance
	{
		static constexpr TypeID kTypeId = 125;
		static constexpr const char* kTypeName = "AttackInstance";
		static constexpr int kVersion = 0;

		// 攻撃情報
		Entity owner;
		int attackIndex;
		bool connected;

		// 終了フラグ及び終了条件フラグ
		bool ended;
		bool endWithOwnerAction;

		// 継続時間
		float elapsedTime;
		float maxDuration;
		AttackInstance()
			: AttackInstance(kInvalidEntity, 0, 0.0f, true)
		{
		}

		AttackInstance(
			Entity a_owner,
			int a_attackIndex,
			float a_maxDuration,
			bool a_endWithOwnerAction = true)
			: owner(a_owner)
			, attackIndex(a_attackIndex)
			, connected(false)
			, ended(false)
			, elapsedTime(0.0f)
			, maxDuration(a_maxDuration)
			, endWithOwnerAction(a_endWithOwnerAction)
		{
		}
	};

	struct AIRole
	{
		static constexpr TypeID kTypeId = 126;
		static constexpr const char* kTypeName = "AIRole";
		static constexpr int kVersion = 0;

		// 距離による有利判定係数（得意間合い）
		float distCoefficient = 1.0f;
		// 様子見用・距離の遠さ係数
		float farnessCoefficient = 1.0f;
		// 攻撃命中時の評価係数
		float hitCoefficient = 3.0f;
		// 方針連続時の減少係数、1.0f以下だと連続時に増加する
		float stanceCoefficient = 1.2f;

		// 有利距離用係数、攻撃命中係数、方針連続使用係数、様子見距離用係数
		AIRole(float a_distCoefficient, float a_hitCoefficient, float a_stanceCoefficient, float a_farnessCoefficient)
			: distCoefficient(a_distCoefficient)
			, farnessCoefficient(a_farnessCoefficient)
			, hitCoefficient(a_hitCoefficient)
			, stanceCoefficient(a_stanceCoefficient)
		{
		}

		AIRole()
			: AIRole(1.0f, 3.0f, 1.2f, 1.0f)
		{
		}
	};

	struct GuardAction
	{
		static constexpr TypeID kTypeId = 127;
		static constexpr const char* kTypeName = "GuardAction";
		static constexpr int kVersion = 0;

		float currentGuardPower;
		float knockbackRate;

		GuardAction(float a_guardPower, float a_knockbackRate)
			: currentGuardPower(a_guardPower)
			, knockbackRate(a_knockbackRate)
		{
		}

		GuardAction()
			: currentGuardPower(0.0f)
			, knockbackRate(1.0f)
		{

		}
	};
	
	struct GuardState
	{
		static constexpr TypeID kTypeId = 128;
		static constexpr const char* kTypeName = "GuardState";
		static constexpr int kVersion = 1;
		float guardPower;
		// !!!New!!!
		float knockbackRate;
		GuardState()
			: guardPower(0.0f)
			, knockbackRate(1.0f)
		{

		}

		GuardState(float a_guardPower, float a_knockbackRate)
			: guardPower(a_guardPower)
			, knockbackRate(a_knockbackRate)
		{
		}
	};

	// !!!New!!!
	struct ClearTarget
	{
		static constexpr TypeID kTypeId = 129;
		static constexpr const char* kTypeName = "ClearTarget";
		static constexpr int kVersion = 0;

		ClearTarget() {};
	};

	struct Name
	{
		static constexpr TypeID kTypeId = 130;
		static constexpr const char* kTypeName = "Name";
		static constexpr int kVersion = 0;

		std::string name;

		Name(std::string a_name)
			: name(a_name)
		{

		}

		Name()
			: name("")
		{

		}
	};

	// !!!New!!!
	struct KnockbackAction
	{
		static constexpr TypeID kTypeId = 131;
		static constexpr const char* kTypeName = "KnockbackAction";
		static constexpr int kVersion = 0;

		float2 moveDir;
		float speed;
		float maxKnockbackTime;
		float elapsedTime;

		KnockbackAction()
			: KnockbackAction(float2(), 0.0f, 0.0f, 0.0f)
		{
		}

		KnockbackAction(float2 a_moveDir, float a_speed, float a_maxKnockbackTime, float a_elapsedTime = 0.0f)
			: moveDir(a_moveDir)
			, speed(a_speed)
			, maxKnockbackTime(a_maxKnockbackTime)
			, elapsedTime(a_elapsedTime)
		{
		}
	};

	// !!!New!!!
	struct InterferenceResult
	{
		static constexpr TypeID kTypeId = 132;
		static constexpr const char* kTypeName = "InterferenceResult";
		static constexpr int kVersion = 0;

		bool useKnockback;
		float2 knockBackDir;
		float knockbackTime;

		InterferenceResult()
			: useKnockback(false)
			, knockBackDir(float2(0.0f, 0.0f))
			, knockbackTime(0.0f)
		{
		}
	};

	struct UseGhostShader
	{
		static constexpr TypeID kTypeId = 133;
		static constexpr const char* kTypeName = "UseGhostShader";
		static constexpr int kVersion = 0;

		UseGhostShader()
		{
		}
	};

	// !!!New!!!
	struct LookOnState
	{
		static constexpr TypeID kTypeId = 134;
		static constexpr const char* kTypeName = "LookOnState";
		static constexpr int kVersion = 0;

		float captureRadius;
		float releaseRadius;
		bool stickSwitchLatch;

		LookOnState()
			: LookOnState(6.8f, 7.8f)
		{
		}

		LookOnState(float a_captureRadius, float a_releaseRadius)
			: captureRadius(a_captureRadius)
			, releaseRadius(a_releaseRadius)
			, stickSwitchLatch(false)
		{
		}
	};

	// !!!New!!!
	struct LookOnAction
	{
		static constexpr TypeID kTypeId = 135;
		static constexpr const char* kTypeName = "LookOnAction";
		static constexpr int kVersion = 0;

		Entity target;
		float3 finalRot;

		LookOnAction()
			: LookOnAction(kInvalidEntity)
		{
		}

		LookOnAction(Entity a_target)
			: target(a_target)
			, finalRot()
		{
		}
	};

	struct LookOnMarkerTag
	{
		static constexpr TypeID kTypeId = 136;
		static constexpr const char* kTypeName = "LookOnMarkerTag";
		static constexpr int kVersion = 0;

		LookOnMarkerTag()
		{
		}
	};

	// FollowPosition等をしている際、追従を付け外ししたくなったりした際にいちいち
	struct NullTargetTag
	{

	};
}


using namespace Component;

//class ComponentsAPI
//{
//
//private:
//	std::map<TypeID, std::string> map;
//	ComponentsAPI()
//	{
//#define ADDMAP(T) map.insert({T::kTypeId, T::kTypeName});
//		COMPONENT_TYPE_LIST(ADDMAP);
//	}
//
//
//public:
//	ComponentsAPI(ComponentsAPI&) = delete;
//	ComponentsAPI& operator=(ComponentsAPI&) = delete;
//	ComponentsAPI(ComponentsAPI&&) = delete;
//	ComponentsAPI& operator=(ComponentsAPI&&) = delete;
//
//	static std::string IDToName(TypeID id)
//	{
//		static ComponentsAPI api;
//		std::string ret;
//		if (api.map.find(id) != api.map.end())
//		{
//			return api.map.at(id);
//		}
//		return "None";
//	}
//};

template<typename T>
inline void CreateImGuiValue(std::string name, T& value, const T& valueDefault) 
{
	ImGui::Text(name.c_str()); 
}

inline void CreateImGuiValue(std::string name, std::string& value, const std::string& valueDefault)
{
	char* newString = new char[100]();
	value.copy(newString, 100);
	ImGui::InputText(name.c_str(), newString, 100);
	value = newString;
	delete[] newString;
}


inline void CreateImGuiValue(std::string name, float& value, const float& valueDefault)
{
	ImGui::InputFloat(name.c_str(), &value);
}

inline void CreateImGuiValue(std::string name, int& value, const int& valueDefault)
{
	ImGui::InputInt(name.c_str(), &value);
}

inline void CreateImGuiValue(std::string name, size_t& value, const size_t& valueDefault)
{
	ImGui::Text(name.c_str());
}

inline void CreateImGuiValue(std::string name, bool& value, const bool& valueDefault)
{
	ImGui::Checkbox(name.c_str(), &value);
}



inline void CreateImGuiEntity(std::string name, Entity& entity)
{
	ImGui::Text(name.c_str());
}