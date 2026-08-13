#include "BulletSystem.h"
#include "Components.h"
#include "Sound.h"
#include "SystemAssist.h"
void ShotWaitUISet(Chunk& a_chunk);
void CanShotUISet(Chunk& a_chunk);
void ResetShotUI(Entity a_target, Chunk& a_chunk);

using namespace Component;

//void LaserSystem(Chunk& a_chunk, const SystemContext& a_context)
//{
//	ComponentView rayInfoView = a_chunk.GetView<ComponentTypes<LaserOwnerTag, Position, RayInfomation>>();
//
//
//	for (auto rayIt : rayInfoView)
//	{
//		const ComponentHandle<RayInfomation> rayHandle = a_chunk.GetComponent<RayInfomation>(rayIt);
//
//		float3 rayHitPos = rayHandle.Look().rayHitPosition;
//		float3 rayHitRot = rayHandle.Look().rayHitRotation;
//
//		ComponentView laserLookView = a_chunk.GetView<ComponentTypes<LookLaserPoint, Position, Rotation, Scale>>();
//
//		for (auto it : laserLookView)
//		{
//			ComponentHandle<LookLaserPoint> look = a_chunk.GetComponent<LookLaserPoint>(it);
//			// 自分の対応するレーザーじゃなければ抜ける
//			if (look.Look().laserOwner != rayIt) return;
//			ComponentHandle<Position> lookPos = a_chunk.GetComponent<Position>(it);
//			ComponentHandle<Rotation> lookRotation = a_chunk.GetComponent<Rotation>(it);
//
//			float3 modelPos(
//				lookPos.Look().x,
//				lookPos.Look().y,
//				lookPos.Look().z
//			);
//
//			float3 dir = {
//				rayHitPos.x - modelPos.x,
//				rayHitPos.y - modelPos.y,
//				rayHitPos.z - modelPos.z
//			};
//			float distXZ = sqrtf(dir.x * dir.x + dir.z * dir.z);
//
//
//
//			if (distXZ > look.Look().minDistance)
//			{
//
//				float3 lookRotatoRad = TOFLOAT3(lookRotation.Look()) * RAD;
//				float3 targetRotatoRad(
//					atan2f(-dir.y, distXZ),
//					atan2f(dir.x, dir.z),
//					0.0f
//				);
//
//				float3 rot = ExponentialRad(lookRotatoRad, targetRotatoRad, 100.5f, a_context.deltaTime);
//
//				lookRotation->pitch = rot.x * DEG;
//				lookRotation->yaw = rot.y * DEG;
//				lookRotation->roll = 0.0f;
//			}
//			else
//			{
//				ComponentHandle<Rotation> ownerRotato = a_chunk.GetComponent<Rotation>(look.Look().laserOwner);
//				if (ownerRotato.IsValid())
//				{
//					//float3 lookRotatoRad = TOFLOAT3(lookRotation.Look()) * RAD;
//					//float3 ownerRotatoRad = TOFLOAT3(ownerRotato.Look()) * RAD;
//
//					//float3 rot = ExponentialRad(lookRotatoRad, ownerRotatoRad, 0.5f, a_context.deltaTime);
//
//					//lookRotation->pitch = rot.x * DEG;
//					//lookRotation->yaw = rot.y * DEG;
//					//lookRotation->roll = 0.0f;
//
//					lookRotation->pitch = ownerRotato->pitch;
//					lookRotation->yaw = ownerRotato->yaw;
//					lookRotation->roll = ownerRotato->roll;
//				}
//
//			}
//
//
//			ComponentHandle<LaserTag> laserTag = a_chunk.GetComponent<LaserTag>(it);
//			if (laserTag.IsValid())
//			{
//				ComponentHandle<Scale> laserScale = a_chunk.GetComponent<Scale>(it);
//				laserScale->z = NormalizeLength(
//					dir.x,
//					dir.y,
//					dir.z
//				);
//			}
//		}
//
//		ComponentView laserPointView = a_chunk.GetView<ComponentTypes<LaserPointTag, Position, Rotation>>();
//		for (auto it : laserPointView)
//		{
//			ComponentHandle<Position> laserPointPosition = a_chunk.GetComponent<Position>(it);
//			ComponentHandle<Rotation> laserPointRotation = a_chunk.GetComponent<Rotation>(it);
//
//			laserPointPosition->x = rayHitPos.x;
//			laserPointPosition->y = rayHitPos.y;
//			laserPointPosition->z = rayHitPos.z;
//			laserPointRotation->pitch = rayHitRot.x;
//			laserPointRotation->yaw = rayHitRot.y;
//			laserPointRotation->roll = rayHitRot.z;
//		}
//	}
//}
//
//void BulletSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_systemResponse)
//{
//	// 撃つボタンが押されていなかったら撃たない
//
//	bool isShotKey = a_context.input.IsRegisterTrigger("Shot");
//
//	ComponentView view = a_chunk.GetView<ComponentTypes<ShooterComponent, Position, Rotation, RayInfomation>>();
//
//	// 全てのShootterを捜査
//	for (auto it : view)
//	{
//
//		ComponentHandle<ShooterComponent> shooter = a_chunk.GetComponent<ShooterComponent>(it);
//		bool isShot = false;
//
//		if (!shooter.Look().canShot)
//		{
//			shooter->shotTimer -= a_context.deltaTime;
//			if (shooter.Look().shotTimer > 0.0f) continue;
//			shooter->canShot = true;
//
//			CanShotUISet(a_chunk);
//			continue;
//		}
//
//		if (!isShotKey) continue;
//
//		PlaySound(LoadSound("Assets/Sound/shot.mp3"));
//		isShot = true;
//
//		// 射撃処理
//		shooter->canShot = false;
//
//		const ComponentHandle<Position> position = a_chunk.GetComponent<Position>(it);
//		const ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(it);
//
//		ComponentView gunView = a_chunk.GetView<ComponentTypes<GunTag>>();
//		ComponentHandle<Position> gunPos;
//		ComponentHandle<Rotation> gunRot;
//
//		for (auto it : gunView)
//		{
//			ComponentHandle<Position> gunPos = a_chunk.GetComponent<Position>(it);
//			ComponentHandle<Rotation> gunRot = a_chunk.GetComponent<Rotation>(it);
//
//			if (!gunPos.IsValid() || !gunRot.IsValid()) continue;
//
//			Entity fire = a_chunk.CreateNewEntity(
//				MOVE_AND_TRANSFORM_COMPONENT(
//					float3(position.Look().x, position.Look().y, position.Look().z),
//					float3(gunRot.Look().pitch, gunRot.Look().yaw, gunRot.Look().roll),
//					float3(0.12f, 0.12f, 0.15f)
//				),
//				ModelKey("FireAnimation"),
//				LifeTime(3.0f),
//				FollowPosition(-0.25f, 1.0f, 6.2f, it, FOLLOW_POS_LOCALOFFSET),
//				FollowRotation(65.0f, -7.5f, 0.0f, it),
//				ShakeComponent(float3(0.01f, 0.01f, 0.025f), float3(0.2f, 0.2f, 0.2f), 30.0f),
//				PosePosState(POSE_POS_FOLLOW),
//				PoseRotState(POSE_ROT_FOLLOW)
//			);
//
//			break;
//		}
//
//
//		// 撃ったかどうかの判定
//		ComponentView aview = a_chunk.GetView<ComponentTypes<PlayerTag, RayInfomation>>();
//		bool isHit = false;
//		bool isEnemyHit = false;
//
//		for (auto it : aview)
//		{
//			const ComponentHandle<RayInfomation> info = a_chunk.GetComponent<RayInfomation>(it);
//
//
//			Entity triggerEntity = info.Look().otherEntity;
//
//			const ComponentHandle<EnemyTag> enemyTag = a_chunk.GetComponent<EnemyTag>(triggerEntity);
//			const ComponentHandle<EnemyBulletTag> enemyBulletTag = a_chunk.GetComponent<EnemyBulletTag>(triggerEntity);
//
//			if (enemyTag.IsValid() || enemyBulletTag.IsValid())
//			{
//				// 敵の撃破画面エフェクトを出す
//				a_systemResponse.AddStopTime(20.0f, 20.0f, 0.1f);
//				//Entity createEffect = a_chunk.CreateNewEntity(CreateEffect(SPEEDLINEFADE));
//
//				ComponentHandle<DeadState> dead = a_chunk.GetComponent<DeadState>(triggerEntity);
//				if (dead.IsValid()) dead->isDead = true;
//
//				// 敵の撃破エフェクトを出す
//				a_chunk.AddComponent(triggerEntity, ShakeComponent(float3(0.15f, 0.15f, 0.15f), float3(2.0f, 2.0f, 2.0f), 50.0f));
//
//				Entity railEntity = GetRail(a_chunk);
//				ComponentHandle<RailComponent> rail = a_chunk.GetComponent<RailComponent>(railEntity);
//
//
//
//
//				float3 shotEffectSpawnPos = info.Look().rayHitPosition - rail.Look().currentPos;
//				ComponentHandle<Position> shooterPos = a_chunk.GetComponent<Position>(it);
//
//				float hitLength = GetLength(TOFLOAT3(shooterPos.Look()), info.Look().rayHitPosition);
//
//				//a_chunk.CreateNewEntity(
//				//	MOVE_AND_TRANSFORM_COMPONENT(
//				//		float3(shotEffectSpawnPos),
//				//		float3(0.0f, 0.0f, 0.0f),
//				//		float3(1.0f, 1.0f, 1.0f)
//				//	),
//				//	PosePosState(POSE_POS_RAIL),
//				//	RailUser(railEntity),
//				//	UIComponent("HitEffectRay", float2(), float2(0.07f * hitLength, 0.07f * hitLength), 0.0f, ),
//				//	SpriteComponent(float3(), float3(), true),
//				//	FadeUI(FADE_DOWN, 0.07f),
//				//	LifeTime(2.0f)
//				//);
//
//				//a_chunk.CreateNewEntity(
//				//	MOVE_AND_TRANSFORM_COMPONENT(
//				//		float3(shotEffectSpawnPos),
//				//		float3(0.0f, 0.0f, 0.0f),
//				//		float3(15.0f, 15.0f, 15.0f)
//				//	),
//				//	PosePosState(POSE_POS_RAIL),
//				//	RailUser(railEntity),
//				//	SpriteComponent("ShotEffect", float3(), float2(1.25f, 1.25f), float3(), 1.0f, true),
//				//	SpriteAnimation(6 + 1, int2(5, 2), false),
//				//	LifeTime(30.0f)
//				//);
//
//				// フラグを立てる
//				isHit = true;
//				isEnemyHit = enemyTag.IsValid();
//			}
//
//		}
//
//		if (isHit)
//		{
//			ComponentView view = a_chunk.GetView<ComponentTypes<ZoomComponent, ZoomMove>>();
//
//			for (auto it : view)
//			{
//				ComponentHandle<ZoomMove> move = a_chunk.GetComponent<ZoomMove>(it);
//
//				move->zoomSpeed = 0.3f;
//				move->zoomF = -0.05f;
//			}
//
//			PlaySound(LoadSound("Assets/Sound/shothit.mp3"));
//			if (isEnemyHit)
//			{
//				PlaySound(LoadSound("Assets/Sound/kill.mp3"));
//			}
//
//			Entity createEffect = a_chunk.CreateNewEntity(
//				CreateEffect(SHOTFLASH)
//			);
//
//		}
//		else
//		{
//			shooter->shotTimer = shooter.Look().maxShotTimer;
//			ShotWaitUISet(a_chunk);
//
//		}
//		
//	}
//}
//
//
//
//void ShotWaitUISet(Chunk& a_chunk)
//{
//	// 射撃待機UIを出す
//	ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent, ShotUI>>();
//	for (auto it : view)
//	{
//		ComponentHandle<ShotUI> shotUI = a_chunk.GetComponent<ShotUI>(it);
//		size_t type = shotUI.Look().uiType;
//		ComponentHandle<UIComponent> shotUIComponent = a_chunk.GetComponent<UIComponent>(it);
//		// 射撃待機UIだった場合表示
//		if (type & SHOTUI_WAIT)
//		{
//			shotUIComponent->isActiv = true;
//		}
//		// 射撃可能UIだった場合非表示
//		else if (type & SHOTUI_CANSHOT)
//		{
//			shotUIComponent->isActiv = false;
//		}
//
//		// リセットする
//		ResetShotUI(it, a_chunk);
//
//	}
//}
//
//void CanShotUISet(Chunk& a_chunk)
//{
//	// 射撃待機UIを出す
//	ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent, ShotUI>>();
//	for (auto it : view)
//	{
//		ComponentHandle<ShotUI> shotUI = a_chunk.GetComponent<ShotUI>(it);
//		size_t type = shotUI.Look().uiType;
//		ComponentHandle<UIComponent> shotUIComponent = a_chunk.GetComponent<UIComponent>(it);
//		// 射撃待機UIだった場合表示
//		if (type & SHOTUI_WAIT)
//		{
//			shotUIComponent->isActiv = false;
//		}
//		// 射撃可能UIだった場合非表示
//		else if (type & SHOTUI_CANSHOT)
//		{
//			shotUIComponent->isActiv = true;
//		}
//
//		// リセットする
//		ResetShotUI(it, a_chunk);
//
//	}
//}
//
//
//void ResetShotUI(Entity a_target, Chunk& a_chunk)
//{
//	ComponentHandle<ShotUI> shotUI = a_chunk.GetComponent<ShotUI>(a_target);
//	size_t type = shotUI.Look().uiType;
//	ComponentHandle<UIComponent> shotUIComponent = a_chunk.GetComponent<UIComponent>(a_target);
//
//	if (type & SHOTUI_FADERESET_UP)
//	{
//		ComponentHandle<FadeUI> fade = a_chunk.GetComponent<FadeUI>(a_target);
//		if (fade.IsValid())
//		{
//			fade->fadeType = FADE_UP;
//			shotUIComponent->alpha = 0.0f;
//		}
//	}
//	else if (type & SHOTUI_FADERESET_DOWN)
//	{
//		ComponentHandle<FadeUI> fade = a_chunk.GetComponent<FadeUI>(a_target);
//		if (fade.IsValid())
//		{
//			fade->fadeType = FADE_DOWN;
//			shotUIComponent->alpha = 1.0f;
//		}
//	}
//}


