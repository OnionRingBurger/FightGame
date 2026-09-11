#include "Components.h"
#include "ECSTypes.h"

using namespace Component;

namespace ComponentSystem
{
	template<typename Component, typename ValueFunc, typename EntityFunc>
	void ApplyToFields(Component& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc) {};

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(PlayerTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EnemyTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DarkTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(GoalTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(CameraTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ItemTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LaserPointTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LaserOwnerTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LaserTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(PlayerViewTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DontHitRayTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(GunTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Position& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("x", component.x, 0.0f);
		valueFunc("y", component.y, 0.0f);
		valueFunc("z", component.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Rotation& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("pitch", component.pitch, 0.0f);
		valueFunc("yaw", component.yaw, 0.0f);
		valueFunc("roll", component.roll, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Scale& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("x", component.x, 0.0f);
		valueFunc("y", component.y, 0.0f);
		valueFunc("z", component.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LookLaserPoint& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("laserOwner", component.laserOwner);
		valueFunc("minDistance", component.minDistance, 0.0f);
		valueFunc("offsetX", component.offset.x, 0.0f);
		valueFunc("offsetY", component.offset.y, 0.0f);
		valueFunc("offsetZ", component.offset.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(InputMove& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("moveSpeedX", component.moveSpeed.x, 0.0f);
		valueFunc("moveSpeedY", component.moveSpeed.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(InputRotato& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("mouseRotatoSpeedX", component.mouseRotatoSpeed.x, 0.0f);
		valueFunc("mouseRotatoSpeedY", component.mouseRotatoSpeed.y, 0.0f);
		valueFunc("padRotatoSpeedX", component.padRotatoSpeed.x, 0.0f);
		valueFunc("padRotatoSpeedY", component.padRotatoSpeed.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ShakingComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isWait", component.isWait, false);
		valueFunc("shankingDegree", component.shankingDegree, 0.0f);
		valueFunc("heightOffset", component.heightOffset, 0.0f);
		valueFunc("shakingSpeed", component.shakingSpeed, 0.0f);
		valueFunc("shakingDisplacement", component.shakingDisplacement, 0.0f);
		valueFunc("shakingType", component.shakingType, static_cast<decltype(component.shakingType)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(PlayerWalkTimer& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("maxTime", component.maxTime, 0.0f);
		valueFunc("time", component.time, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EnemySpawner& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("spawnTime", component.spawnTime, 0.0f);
		valueFunc("maxSpawnTime", component.maxSpawnTime, 0.0f);
		valueFunc("flag", component.flag, static_cast<decltype(component.flag)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Timer& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("time", component.time, 0.0f);
		valueFunc("maxTime", component.maxTime, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LookComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("target", component.target);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Velocity& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("x", component.x, 0.0f);
		valueFunc("y", component.y, 0.0f);
		valueFunc("z", component.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Force& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("x", component.force.x, 0.0f);
		valueFunc("y", component.force.y, 0.0f);
		valueFunc("z", component.force.z, 0.0f);
		valueFunc("attenuation", component.attenuation, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(BoxCollider& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("offsetX", component.offset.x, 0.0f);
		valueFunc("offsetY", component.offset.y, 0.0f);
		valueFunc("offsetZ", component.offset.z, 0.0f);
		valueFunc("collisionScaleX", component.collisionScale.x, 0.0f);
		valueFunc("collisionScaleY", component.collisionScale.y, 0.0f);
		valueFunc("collisionScaleZ", component.collisionScale.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(MoveForward& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("speed", component.speed, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ShooterComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("shotSpeed", component.shotSpeed, 0.0f);
		valueFunc("shotTimer", component.shotTimer, 0.0f);
		valueFunc("maxShotTimer", component.maxShotTimer, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(GhostAreaComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("length", component.length, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(OBBCollider& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("centerX", component.center.x, 0.0f);
		valueFunc("centerY", component.center.y, 0.0f);
		valueFunc("centerZ", component.center.z, 0.0f);
		valueFunc("halfX", component.half.x, 0.0f);
		valueFunc("halfY", component.half.y, 0.0f);
		valueFunc("halfZ", component.half.z, 0.0f);
		valueFunc("obbBitFlag", component.obbBitFlag, static_cast<decltype(component.obbBitFlag)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(TrackingWarp& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("target", component.targetEntity);
		valueFunc("warpDisX", component.warpDis.x, 0.0f);
		valueFunc("warpDisY", component.warpDis.y, 0.0f);
		valueFunc("warpDisZ", component.warpDis.z, 0.0f);
		valueFunc("trackOffsetX", component.trackOffset.x, 0.0f);
		valueFunc("trackOffsetY", component.trackOffset.y, 0.0f);
		valueFunc("trackOffsetZ", component.trackOffset.z, 0.0f);
		valueFunc("flag", component.flag, static_cast<decltype(component.flag)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(HitInfomation& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ChunkChange& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isWait", component.isWait, false);
		valueFunc("chunkType", component.chunkType, std::string(""));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(KeyChunkChange& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("key", component.key, std::string(""));
		valueFunc("waitTime", component.waitTime, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(CreateEffect& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("type", component.type, static_cast<decltype(component.type)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ZoomComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("zoom", component.zoom, 0.0f);
		valueFunc("maxZoom", component.maxZoom, 0.0f);
		valueFunc("minZoom", component.minZoom, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ZoomMove& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("moveType", component.moveType, static_cast<decltype(component.moveType)>(0));
		valueFunc("zoomSpeed", component.zoomSpeed, 0.0f);
		valueFunc("zoomF", component.zoomF, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EffectKey& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("type", component.type, static_cast<decltype(component.type)>(0));
		valueFunc("key", component.key, std::string(""));
		valueFunc("isRecycling", component.isRecycling, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DelayChunkChange& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("maxDelayTime", component.maxDelayTime, 0.0f);
		valueFunc("currentCount", component.currentCount, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(FadeUI& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("fadeType", component.fadeType, static_cast<decltype(component.fadeType)>(0));
		valueFunc("fadeSpeed", component.fadeSpeed, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(FadeChange& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("type", component.type, static_cast<decltype(component.type)>(0));
		valueFunc("isWait", component.isWait, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(OwnerComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("owner", component.owner);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(BulletComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("damage", component.damage, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Camera& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("priority", component.cameraPriority, static_cast<decltype(component.cameraPriority)>(0));
		valueFunc("lookPosX", component.lookPosition.x, 0.0f);
		valueFunc("lookPosY", component.lookPosition.y, 0.0f);
		valueFunc("lookPosZ", component.lookPosition.z, 0.0f);
		valueFunc("upVectorX", component.upVector.x, 0.0f);
		valueFunc("upVectorY", component.upVector.y, 0.0f);
		valueFunc("upVectorZ", component.upVector.z, 0.0f);
		valueFunc("fovy", component.fovy, 0.0f);
		valueFunc("aspect", component.aspect, 0.0f);
		valueFunc("nearClip", component.nearCrip, 0.0f);
		valueFunc("farClip", component.farCrip, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(CameraPoint& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("cameraPointPriority", component.cameraPointPriority, static_cast<decltype(component.cameraPointPriority)>(0));
		valueFunc("posLeapSpeed", component.posLeapSpeed, 0.0f);
		valueFunc("posOffsetX", component.posOffset.x, 0.0f);
		valueFunc("posOffsetY", component.posOffset.y, 0.0f);
		valueFunc("posOffsetZ", component.posOffset.z, 0.0f);
		valueFunc("rotLeapSpeed", component.rotLeapSpeed, 0.0f);
		valueFunc("rotOffsetX", component.rotOffset.x, 0.0f);
		valueFunc("rotOffsetY", component.rotOffset.y, 0.0f);
		valueFunc("rotOffsetZ", component.rotOffset.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(FollowPosition& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("offsetX", component.positionOffset.x, 0.0f);
		valueFunc("offsetY", component.positionOffset.y, 0.0f);
		valueFunc("offsetZ", component.positionOffset.z, 0.0f);
		entityFunc("target", component.targetEntity);
		valueFunc("flag", component.flag, static_cast<decltype(component.flag)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(FollowRotation& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("rotationOffsetX", component.rotationOffset.x, 0.0f);
		valueFunc("rotationOffsetY", component.rotationOffset.y, 0.0f);
		valueFunc("rotationOffsetZ", component.rotationOffset.z, 0.0f);
		entityFunc("target", component.targetEntity);
		valueFunc("flag", component.flag, static_cast<decltype(component.flag)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Ray& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("rayOffsetX", component.rayOffset.x, 0.0f);
		valueFunc("rayOffsetY", component.rayOffset.y, 0.0f);
		valueFunc("rayOffsetZ", component.rayOffset.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(RayInfomation& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("rayOwner", component.rayOwner);
		entityFunc("otherEntity", component.otherEntity);
		valueFunc("rayHitPositionX", component.rayHitPosition.x, 0.0f);
		valueFunc("rayHitPositionY", component.rayHitPosition.y, 0.0f);
		valueFunc("rayHitPositionZ", component.rayHitPosition.z, 0.0f);
		valueFunc("rayHitRotationX", component.rayHitRotation.x, 0.0f);
		valueFunc("rayHitRotationY", component.rayHitRotation.y, 0.0f);
		valueFunc("rayHitRotationZ", component.rayHitRotation.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LifeTime& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("time", component.time, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(TrailComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("id", component.id, static_cast<decltype(component.id)>(0));
		valueFunc("shrinkageRate", component.shrinkageRate, 0.0f);
		valueFunc("size", component.size, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ModelKey& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("key", component.key, std::string(""));
		valueFunc("offsetX", component.offset.x, 0.0f);
		valueFunc("offsetY", component.offset.y, 0.0f);
		valueFunc("offsetZ", component.offset.z, 0.0f);
		valueFunc("rotateX", component.rotato.x, 0.0f);
		valueFunc("rotateY", component.rotato.y, 0.0f);
		valueFunc("rotateZ", component.rotato.z, 0.0f);
		valueFunc("defaultRotateX", component.defaultRotato.x, 0.0f);
		valueFunc("defaultRotateY", component.defaultRotato.y, 0.0f);
		valueFunc("defaultRotateZ", component.defaultRotato.z, 0.0f);
		valueFunc("flag", component.modelDrawFlag, static_cast<decltype(component.modelDrawFlag)>(0));
		valueFunc("useAnime", component.useAnime, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(UIComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("key", component.key, std::string(""));
		valueFunc("uiPosX", component.uiPos.x, 0.0f);
		valueFunc("uiPosY", component.uiPos.y, 0.0f);
		valueFunc("uiScaleX", component.uiScale.x, 0.0f);
		valueFunc("uiScaleY", component.uiScale.y, 0.0f);
		valueFunc("uiRotation", component.uiRotation, 0.0f);
		valueFunc("alpha", component.alpha, 0.0f);
		valueFunc("uvPosX", component.uvPos.x, 0.0f);
		valueFunc("uvPosY", component.uvPos.y, 0.0f);
		valueFunc("uvScaleX", component.uvScale.x, 1.0f);
		valueFunc("uvScaleY", component.uvScale.y, 1.0f);
		valueFunc("isActiv", component.isActiv, true);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(SpriteComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("offsetPosX", component.offsetPos.x, 0.0f);
		valueFunc("offsetPosY", component.offsetPos.y, 0.0f);
		valueFunc("offsetPosZ", component.offsetPos.z, 0.0f);
		valueFunc("offsetRotationX", component.offsetRotation.x, 0.0f);
		valueFunc("offsetRotationY", component.offsetRotation.y, 0.0f);
		valueFunc("offsetRotationZ", component.offsetRotation.z, 0.0f);
		valueFunc("isBillBoard", component.isBillBoard, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Poliline& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LeapPosComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isActicv", component.isActicv, false);
		valueFunc("leapStartPosX", component.leapStartPos.x, 0.0f);
		valueFunc("leapStartPosY", component.leapStartPos.y, 0.0f);
		valueFunc("leapStartPosZ", component.leapStartPos.z, 0.0f);
		valueFunc("leapEndPosX", component.leapEndPos.x, 0.0f);
		valueFunc("leapEndPosY", component.leapEndPos.y, 0.0f);
		valueFunc("leapEndPosZ", component.leapEndPos.z, 0.0f);
		valueFunc("leapTime", component.leapTime, 0.0f);
		valueFunc("leapMaxTime", component.leapMaxTime, 0.0f);
		valueFunc("leapRate", component.leapRate, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LeapRotComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isActicv", component.isActicv, false);
		valueFunc("leapStartRotX", component.leapStartRot.x, 0.0f);
		valueFunc("leapStartRotY", component.leapStartRot.y, 0.0f);
		valueFunc("leapStartRotZ", component.leapStartRot.z, 0.0f);
		valueFunc("leapEndRotX", component.leapEndRot.x, 0.0f);
		valueFunc("leapEndRotY", component.leapEndRot.y, 0.0f);
		valueFunc("leapEndRotZ", component.leapEndRot.z, 0.0f);
		valueFunc("leapTime", component.leapTime, 0.0f);
		valueFunc("leapMaxTime", component.leapMaxTime, 0.0f);
		valueFunc("leapRate", component.leapRate, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(FlipComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(UIAngularSpeed& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("speed", component.speed, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Pose& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("posX", component.pos.x, 0.0f);
		valueFunc("posY", component.pos.y, 0.0f);
		valueFunc("posZ", component.pos.z, 0.0f);
		valueFunc("rotX", component.rot.x, 0.0f);
		valueFunc("rotY", component.rot.y, 0.0f);
		valueFunc("rotZ", component.rot.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(PosePosState& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("state", component.state, static_cast<decltype(component.state)>(POSE_POS_NONE));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(PoseRotState& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("state", component.state, static_cast<decltype(component.state)>(POSE_ROT_NONE));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(RailComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isActicv", component.isActicv, false);
		valueFunc("railStartPosX", component.railStartPos.x, 0.0f);
		valueFunc("railStartPosY", component.railStartPos.y, 0.0f);
		valueFunc("railStartPosZ", component.railStartPos.z, 0.0f);
		valueFunc("railEndPosX", component.railEndPos.x, 0.0f);
		valueFunc("railEndPosY", component.railEndPos.y, 0.0f);
		valueFunc("railEndPosZ", component.railEndPos.z, 0.0f);
		valueFunc("currentPosX", component.currentPos.x, 0.0f);
		valueFunc("currentPosY", component.currentPos.y, 0.0f);
		valueFunc("currentPosZ", component.currentPos.z, 0.0f);
		valueFunc("railTime", component.railTime, 0.0f);
		valueFunc("railMaxTime", component.railMaxTime, 0.0f);
		valueFunc("railRate", component.railRate, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(RailUser& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("targetRail", component.targetRail);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(RailFly& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("flyPow", component.flyPow, 0.0f);
		valueFunc("flyProgress", component.flyProgress, 0.0f);
		valueFunc("flySpeed", component.flySpeed, 0.0f);
	}

	// !!!New!!!
	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AngleLimitComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isLimitX", component.isLimitX, false);
		valueFunc("isLimitY", component.isLimitY, false);
		valueFunc("maxX", component.maxAngle.x, 0.0f);
		valueFunc("minX", component.minAngle.x, 0.0f);
		valueFunc("maxY", component.maxAngle.y, 0.0f);
		valueFunc("minY", component.minAngle.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DebugCameraTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DebugInputPos& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("moveSpeedX", component.moveSpeed.x, 0.0f);
		valueFunc("moveSpeedY", component.moveSpeed.y, 0.0f);
		valueFunc("isInput", component.isInput, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DebugInputRot& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("rotatoSpeedX", component.rotatoSpeed.x, 0.0f);
		valueFunc("rotatoSpeedY", component.rotatoSpeed.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EnemyShooter& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("shotTimer", component.shotTimer, 0.0f);
		valueFunc("maxShotWaitTime", component.maxShotWaitTime, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EnemyBulletTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(HitPoint& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("maxHP", component.maxHP, 0.0f);
		valueFunc("currentHP", component.currentHP, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DeadState& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isDead", component.isDead, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(HPGaugeUI& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("target", component.target);
		valueFunc("gaugePosX", component.gaugePos.x, 0.0f);
		valueFunc("gaugePosY", component.gaugePos.y, 0.0f);
		valueFunc("gaugeScaleX", component.gaugeScale.x, 0.0f);
		valueFunc("gaugeScaleY", component.gaugeScale.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ShakeComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("shakeTime", component.shakeTime, 0.0f);
		valueFunc("shakePowerX", component.shakePower.x, 0.0f);
		valueFunc("shakePowerY", component.shakePower.y, 0.0f);
		valueFunc("shakePowerZ", component.shakePower.z, 0.0f);
		valueFunc("shakeAmplitudeX", component.shakeAmplitude.x, 0.0f);
		valueFunc("shakeAmplitudeY", component.shakeAmplitude.y, 0.0f);
		valueFunc("shakeAmplitudeZ", component.shakeAmplitude.z, 0.0f);
		valueFunc("elapsedTime", component.elapsedTime, 0.0f);
		valueFunc("decayRate", component.decayRate, 0.5f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AddDamageComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("damageValue", component.damageValue, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(SpriteAnimation& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("maxSprite", component.maxSprite, 0);
		valueFunc("nextSprite", component.nextSprite, 0);
		valueFunc("spriteUVCountX", component.spriteUVCount.x, 0);
		valueFunc("spriteUVCountY", component.spriteUVCount.y, 0);
		valueFunc("isLoop", component.isLoop, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ShotUI& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("uiType", component.uiType, static_cast<decltype(component.uiType)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(UVMove& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("moveSpeedX", component.moveSpeed.x, 0.0f);
		valueFunc("moveSpeedY", component.moveSpeed.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(MotionTransform& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("motionPosX", component.motionPos.x, 0.0f);
		valueFunc("motionPosY", component.motionPos.y, 0.0f);
		valueFunc("motionPosZ", component.motionPos.z, 0.0f);
		valueFunc("motionRotX", component.motionRot.x, 0.0f);
		valueFunc("motionRotY", component.motionRot.y, 0.0f);
		valueFunc("motionRotZ", component.motionRot.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(FixedResult& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("newPosX", component.newPos.x, 0.0f);
		valueFunc("newPosY", component.newPos.y, 0.0f);
		valueFunc("newPosZ", component.newPos.z, 0.0f);
		valueFunc("newRotX", component.newRot.x, 0.0f);
		valueFunc("newRotY", component.newRot.y, 0.0f);
		valueFunc("newRotZ", component.newRot.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(MotionResult& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("posOffsetX", component.posOffset.x, 0.0f);
		valueFunc("posOffsetY", component.posOffset.y, 0.0f);
		valueFunc("posOffsetZ", component.posOffset.z, 0.0f);
		valueFunc("rotOffsetX", component.rotOffset.x, 0.0f);
		valueFunc("rotOffsetY", component.rotOffset.y, 0.0f);
		valueFunc("rotOffsetZ", component.rotOffset.z, 0.0f);
		valueFunc("isWarp", component.isWarp, false);
		valueFunc("warpPosX", component.warpPos.x, 0.0f);
		valueFunc("warpPosY", component.warpPos.y, 0.0f);
		valueFunc("warpPosZ", component.warpPos.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EphemeralResult& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("posOffsetX", component.posOffset.x, 0.0f);
		valueFunc("posOffsetY", component.posOffset.y, 0.0f);
		valueFunc("posOffsetZ", component.posOffset.z, 0.0f);
		valueFunc("rotOffsetX", component.rotOffset.x, 0.0f);
		valueFunc("rotOffsetY", component.rotOffset.y, 0.0f);
		valueFunc("rotOffsetZ", component.rotOffset.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Firework& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("fireworkType", component.fireworkType, static_cast<decltype(component.fireworkType)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AlphaBlendComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("textureKey1", component.textureKey1, std::string(""));
		valueFunc("textureKey2", component.textureKey2, std::string(""));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AngularVelocity& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("velocityX", component.velocity.x, 0.0f);
		valueFunc("velocityY", component.velocity.y, 0.0f);
		valueFunc("velocityZ", component.velocity.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(SectorHitJudge& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("minLength", component.minLength, 0.0f);
		valueFunc("maxLength", component.maxLength, 0.0f);
		valueFunc("angle", component.angle, 0.0f);
		valueFunc("maxHeight", component.maxHeight, 0.0f);
		valueFunc("maxLowness", component.maxLowness, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(PlayerAttackTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AttackHitRecord& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(GroundedState& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isGrounded", component.isGrounded, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ActionMask& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("allowed", component.allowed, static_cast<decltype(component.allowed)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(JumpAction& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(WorldPower& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("powerX", component.power.x, 0.0f);
		valueFunc("powerY", component.power.y, 0.0f);
		valueFunc("powerZ", component.power.z, 0.0f);
		valueFunc("rateX", component.rate.x, 0.0f);
		valueFunc("rateY", component.rate.y, 0.0f);
		valueFunc("rateZ", component.rate.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(JumpPower& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("initialVelocityX", component.initialVelocity.x, 0.0f);
		valueFunc("initialVelocityY", component.initialVelocity.y, 0.0f);
		valueFunc("initialVelocityZ", component.initialVelocity.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AttackStatus& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AttackAction& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("attackEntity", component.attackEntity);
		valueFunc("elapsedTime", component.elapsedTime, 0.0f);
		valueFunc("attackIndex", component.attackIndex, 0);
		valueFunc("maxDuration", component.maxDuration, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AttackWaitAction& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("elapsedTime", component.elapsedTime, 0.0f);
		valueFunc("waitDuration", component.waitDuration, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LookMove& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("rotateSpeed", component.rotateSpeed, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(MoveInputResult& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("moveDirX", component.moveDir.x, 0.0f);
		valueFunc("moveDirY", component.moveDir.y, 0.0f);
		valueFunc("magnitube", component.magnitube, 0.0f);
		valueFunc("isInput", component.isInput, false);
		valueFunc("useAttack", component.useAttack, false);
		valueFunc("attackIndex", component.attackIndex, 0);
		valueFunc("useJump", component.useJump, false);
		valueFunc("useGuard", component.useGuard, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(InputSource& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("move", component.move, static_cast<decltype(component.move)>(0));
		valueFunc("attack", component.attack, static_cast<decltype(component.attack)>(0));
		valueFunc("jump", component.jump, static_cast<decltype(component.jump)>(0));
		valueFunc("guard", component.guard, static_cast<decltype(component.guard)>(0));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EnemyAttackTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AttackTelegraph& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("minLength", component.minLength, 0.0f);
		valueFunc("maxLength", component.maxLength, 0.0f);
		valueFunc("angle", component.angle, 0.0f);
		valueFunc("maxHeight", component.maxHeight, 0.0f);
		valueFunc("maxLowness", component.maxLowness, 0.0f);
		valueFunc("sectorKey", component.sectorKey, std::string(""));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AttackStartupAction& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("elapsedTime", component.elapsedTime, 0.0f);
		valueFunc("startupDuration", component.startupDuration, 0.0f);
		valueFunc("attackIndex", component.attackIndex, 0);
		entityFunc("telegraphEntity", component.telegraphEntity);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AttackInstance& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("owner", component.owner);
		valueFunc("attackIndex", component.attackIndex, 0);
		valueFunc("connected", component.connected, false);
		valueFunc("ended", component.ended, false);
		valueFunc("endWithOwnerAction", component.endWithOwnerAction, true);
		valueFunc("elapsedTime", component.elapsedTime, 0.0f);
		valueFunc("maxDuration", component.maxDuration, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AIRole& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("distCoefficient", component.distCoefficient, 1.0f);
		valueFunc("farnessCoefficient", component.farnessCoefficient, 1.0f);
		valueFunc("hitCoefficient", component.hitCoefficient, 3.0f);
		valueFunc("stanceCoefficient", component.stanceCoefficient, 1.2f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(GuardAction& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("currentGuardPower", component.currentGuardPower, 0.0f);
		valueFunc("knockbackRate", component.knockbackRate, 1.0f);
		entityFunc("guardEffect", component.guardEffect);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(GuardState& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("guardPower", component.guardPower, 0.0f);
		valueFunc("knockbackRate", component.knockbackRate, 1.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(ClearTarget& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	// !!!New!!!
	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(KnockbackAction& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("moveDirX", component.moveDir.x, 0.0f);
		valueFunc("moveDirY", component.moveDir.y, 0.0f);
		valueFunc("speed", component.speed, 0.0f);
		valueFunc("maxKnockbackTime", component.maxKnockbackTime, 0.0f);
		valueFunc("elapsedTime", component.elapsedTime, 0.0f);
	}

	// !!!New!!!
	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(InterferenceResult& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("useKnockback", component.useKnockback, false);
		valueFunc("knockBackDirX", component.knockBackDir.x, 0.0f);
		valueFunc("knockBackDirY", component.knockBackDir.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(Name& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("name", component.name, std::string(""));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LookOnState& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("captureRadius", component.captureRadius, 6.8f);
		valueFunc("releaseRadius", component.releaseRadius, 7.8f);
		valueFunc("stickSwitchLatch", component.stickSwitchLatch, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LookOnAction& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("target", component.target);
		valueFunc("finalRotX", component.finalRot.x, 0.0f);
		valueFunc("finalRotY", component.finalRot.y, 0.0f);
		valueFunc("finalRotZ", component.finalRot.z, 0.0f);
		valueFunc("targetCooltime", component.targetCooltime, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AttackKeyLoad& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("size", component.size, 0);
		if(component.size != component.attackKeys.size()) component.attackKeys.resize(component.size);
		for (int i = 0; i < component.size; i++)
		{
			valueFunc("attack" + std::to_string(i), component.attackKeys[i], std::string(""));
		}
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(PhaseSpawner& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("size", component.size, 0);
		if (component.size != component.spawnName.size()) component.spawnName.resize(component.size);
		for (int i = 0; i < component.size; i++)
		{
			valueFunc("spawn" + std::to_string(i), component.spawnName[i], std::string(""));
		}
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EfkEffectKey& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("handleId", component.handleId, std::string(""));
		valueFunc("isLoop", component.isLoop, false);
		valueFunc("waitTime", component.waitTime, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EntryAction& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("duration", component.duration, 0.0f);
		valueFunc("maxDuration", component.maxDuration, 0.0f);
		valueFunc("canPlayEffect", component.canPlayEffect, true);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(BossTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(UseGhostShader& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(LookOnMarkerTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(NullTargetTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(SelectCursor& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		entityFunc("selectEntity", component.selectEntity);
		valueFunc("isActiv", component.isActiv, false);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(SelectBox& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("posX", component.pos.x, 0.0f);
		valueFunc("posY", component.pos.y, 0.0f);
		valueFunc("cursorScaleX", component.cursorScale.x, 0.0f);
		valueFunc("cursorScaleY", component.cursorScale.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(CameraRigTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(RigLookOn& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(FollowLeap& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("followLeapX", component.followLeap.x, 0.0f);
		valueFunc("followLeapY", component.followLeap.y, 0.0f);
		valueFunc("followLeapZ", component.followLeap.z, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DebugEntityTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EfkEffectRuntime& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("handle", component.handle, 0);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(UIScaleLerp& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("progress", component.progress, 0.0f);
		valueFunc("maxLerpTime", component.maxLerpTime, 0.0f);
		valueFunc("targetScaleX", component.targetScale.x, 0.0f);
		valueFunc("targetScaleY", component.targetScale.y, 0.0f);
		valueFunc("startScaleX", component.startScale.x, 0.0f);
		valueFunc("startScaleY", component.startScale.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(UIPosLerp& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("progress", component.progress, 0.0f);
		valueFunc("maxLerpTime", component.maxLerpTime, 0.0f);
		valueFunc("targetPosX", component.targetPos.x, 0.0f);
		valueFunc("targetPosY", component.targetPos.y, 0.0f);
		valueFunc("startPosX", component.startPos.x, 0.0f);
		valueFunc("startPosY", component.startPos.y, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DropUI& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		int size = static_cast<int>(component.playSounds.size());
		valueFunc("size", size, 0);
		if (size != static_cast<int>(component.playSounds.size()))
		{
			component.playSounds.resize(size);
		}
		for (int i = 0; i < size; i++)
		{
			valueFunc("play" + std::to_string(i), component.playSounds[i], std::string(""));
		}
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(OtherChunkChangeLock& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(GameOverTarget& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(SoundKey& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isLoop", component.isLoop, false);
		valueFunc("waitTime", component.waitTime, 0.0f);
		valueFunc("soundKey", component.soundKey, std::string(""));
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(EfkEffectArea& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		int size = static_cast<int>(component.spawnEffectKeys.size());
		valueFunc("size", size, 0);
		if (size != static_cast<int>(component.spawnEffectKeys.size()))
		{
			component.spawnEffectKeys.resize(size);
		}
		for (int i = 0; i < size; i++)
		{
			valueFunc("spawnEffectKeys" + std::to_string(i), component.spawnEffectKeys[i], std::string(""));
		}
		valueFunc("spawnRadius", component.spawnRadius, 0.0f);
		valueFunc("spawnInteval", component.spawnInteval, 0.0f);
		valueFunc("currentDuration", component.currentDuration, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(AllActionStopper& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(CameraChangeEffect& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("effectKey", component.effectKey, "");
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(PlayerHeal& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("isHealEnd", component.isHealEnd, false);
		valueFunc("healValue", component.healValue, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DeleteOnInput& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("key", component.key, std::string(""));
		valueFunc("maxWaitTime", component.maxWaitTime, 0.0f);
		valueFunc("elapsedTime", component.elapsedTime, 0.0f);
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(UITextBoxCursor& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("uiKey", component.uiKey, std::string(""));
		valueFunc("uiPosX", component.uiPos.x, 0.0f);
		valueFunc("uiPosY", component.uiPos.y, 0.0f);
		valueFunc("uiScaleX", component.uiScale.x, 0.0f);
		valueFunc("uiScaleY", component.uiScale.y, 0.0f);
		valueFunc("fadeSpeed", component.fadeSpeed, 0.01f);
		valueFunc("fadeMin", component.fadeMin, 0.0f);
		valueFunc("fadeMax", component.fadeMax, 1.0f);
		valueFunc("soundKey", component.soundKey, std::string(""));
	}

};