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
		valueFunc("key", component.key, 0);
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
		valueFunc("key", component.key, 0);
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
		valueFunc("priority", component.cameraPriority, 0);
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
		valueFunc("cameraPointPriority", component.cameraPointPriority, static_cast<size_t>(0));
		valueFunc("posLeapSpeed", component.posLeapSpeed, 0.0f);
		valueFunc("rotLeapSpeed", component.rotLeapSpeed, 0.0f);
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
		valueFunc("flag", component.modelDrawFlag, 0);
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
	}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(SpriteComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
		valueFunc("key", component.key, std::string(""));
		valueFunc("offsetPosX", component.offsetPos.x, 0.0f);
		valueFunc("offsetPosY", component.offsetPos.y, 0.0f);
		valueFunc("offsetPosZ", component.offsetPos.z, 0.0f);
		valueFunc("sizeX", component.size.x, 0.0f);
		valueFunc("sizeY", component.size.y, 0.0f);
		valueFunc("offsetRotationX", component.offsetRotation.x, 0.0f);
		valueFunc("offsetRotationY", component.offsetRotation.y, 0.0f);
		valueFunc("offsetRotationZ", component.offsetRotation.z, 0.0f);
		valueFunc("alpha", component.alpha, 0.0f);
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

	//template<typename ValueFunc, typename EntityFunc>
	//inline void ApplyToFields(RailComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	//{
	//	valueFunc("isActicv", component.isActicv, false);
	//	valueFunc("railStartPosX", component.railStartPos.x, 0.0f);
	//	valueFunc("railStartPosY", component.railStartPos.y, 0.0f);
	//	valueFunc("railStartPosZ", component.railStartPos.z, 0.0f);
	//	valueFunc("railEndPosX", component.railEndPos.x, 0.0f);
	//	valueFunc("railEndPosY", component.railEndPos.y, 0.0f);
	//	valueFunc("railEndPosZ", component.railEndPos.z, 0.0f);
	//	valueFunc("currentPosX", component.currentPos.x, 0.0f);
	//	valueFunc("currentPosY", component.currentPos.y, 0.0f);
	//	valueFunc("currentPosZ", component.currentPos.z, 0.0f);
	//	valueFunc("railTime", component.railTime, 0.0f);
	//	valueFunc("railMaxTime", component.railMaxTime, 0.0f);
	//	valueFunc("railRate", component.railRate, 0.0f);
	//}

	//template<typename ValueFunc, typename EntityFunc>
	//inline void ApplyToFields(RailUser& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	//{
	//	entityFunc("targetRail", component.targetRail);
	//}

	//template<typename ValueFunc, typename EntityFunc>
	//inline void ApplyToFields(RailFly& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	//{
	//	valueFunc("flyPow", component.flyPow, 0.0f);
	//	valueFunc("flyProgress", component.flyProgress, 0.0f);
	//	valueFunc("flySpeed", component.flySpeed, 0.0f);
	//}

	//template<typename ValueFunc, typename EntityFunc>
	//inline void ApplyToFields(RailApproach& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	//{
	//	valueFunc("currentPositionX", component.currentPosition.x, 0.0f);
	//	valueFunc("currentPositionY", component.currentPosition.y, 0.0f);
	//	valueFunc("currentPositionZ", component.currentPosition.z, 0.0f);
	//	valueFunc("speed", component.speed, 0.0f);
	//	valueFunc("isStop", component.isStop, false);
	//}

	//template<typename ValueFunc, typename EntityFunc>
	//inline void ApplyToFields(AngleLimitComponent& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	//{
	//	valueFunc("isLimitX", component.isLimitX, false);
	//	valueFunc("isLimitY", component.isLimitY, false);
	//	valueFunc("maxX", component.maxAngle.x, 0.0f);
	//	valueFunc("minX", component.minAngle.x, 0.0f);
	//	valueFunc("maxY", component.maxAngle.y, 0.0f);
	//	valueFunc("minY", component.minAngle.y, 0.0f);
	//}

	template<typename ValueFunc, typename EntityFunc>
	inline void ApplyToFields(DebugCameraTag& component, ValueFunc&& valueFunc, EntityFunc&& entityFunc)
	{
	}

};