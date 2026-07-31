#include "CoreSystem.h"
#include "Components.h"

using namespace Component;

// TODO Resetは本来ECS側の役目であるため、時間がある時に改修する
void ResetSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView viewHitInfomation = a_chunk.GetView<ComponentTypes<HitInfomation>>();

	for (auto it : viewHitInfomation)
	{
		ComponentHandle<HitInfomation> info = a_chunk.GetComponent<HitInfomation>(it);
		info->hitResults.clear();
		info->triggerResults.clear();
	}

	ComponentView viewRayInfomation = a_chunk.GetView<ComponentTypes<RayInfomation>>();

	for (auto it : viewRayInfomation)
	{
		a_chunk.DeleteChunkComponent(it, RayInfomation::kTypeId);
	}

	ComponentView viewCreateEffect = a_chunk.GetView<ComponentTypes<CreateEffect>>();

	for (auto it : viewCreateEffect)
	{
		a_chunk.DeleteChunkEntity(it);
	}

	ComponentView viewFixedResult = a_chunk.GetView<ComponentTypes<FixedResult>>();
	for (auto it : viewFixedResult)
	{
		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(it);

		result->newPos = float3(0.0f, 0.0f, 0.0f);
		result->newRot = float3(0.0f, 0.0f, 0.0f);
	}


	ComponentView viewMotionResult = a_chunk.GetView<ComponentTypes<MotionResult>>();

	for (auto it : viewMotionResult)
	{
		ComponentHandle<MotionResult> result = a_chunk.GetComponent<MotionResult>(it);
		result->isWarp = false;
		result->warpPos = float3(0.0f, 0.0f, 0.0f);
		result->posOffset = float3(0.0f, 0.0f, 0.0f);
		result->rotOffset = float3(0.0f, 0.0f, 0.0f);
	}

	ComponentView viewEphemeralResult = a_chunk.GetView<ComponentTypes<EphemeralResult>>();

	for (auto it : viewEphemeralResult)
	{
		ComponentHandle<EphemeralResult> result = a_chunk.GetComponent<EphemeralResult>(it);
		result->posOffset = float3(0.0f, 0.0f, 0.0f);
		result->rotOffset = float3(0.0f, 0.0f, 0.0f);
	}

	// !!!New!!!
	ComponentView viewMoveInputResult = a_chunk.GetView<ComponentTypes<MoveInputResult>>();
	for (auto it : viewMoveInputResult)
	{
		ComponentHandle<MoveInputResult> result = a_chunk.GetComponent<MoveInputResult>(it);
		result->moveDir = float2(0.0f, 0.0f);
		result->magnitube = 0.0f;
		result->isInput = false;
		result->useAttack = false;
	}
}

void LifeTimeSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<LifeTime>>();

	for (auto it : view)
	{
		ComponentHandle time = a_chunk.GetComponent<LifeTime>(it);
		time->time -= a_context.deltaTime;

		if (time.Look().time <= 0.0f)
		{
			a_chunk.DeleteChunkEntity(it);
		}
	}
}

void RaySystem(Chunk& a_chunk, const SystemContext& a_context)
{

	// 何度もviewを回さないように最初に確保
	ComponentView colliderView = a_chunk.GetView<ComponentTypes<BoxCollider, Position, Rotation>, ComponentTypes<DontHitRayTag>>();
	std::vector<Entity> colliderEntities;
	for (auto it : colliderView)
	{
		colliderEntities.push_back(it);
	}

	// Rayの判定を行う
	ComponentView view = a_chunk.GetView<ComponentTypes<Ray, Position, Rotation>>();
	for (auto it : view)
	{
		// RayのComponentを取得
		const ComponentHandle<Ray> ray = a_chunk.GetComponent<Ray>(it);
		const ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
		const ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(it);


		// 行列を使って注視点を求める
		float pitch = rot.Look().pitch * RAD;
		float yaw = rot.Look().yaw * RAD;
		float roll = rot.Look().roll * RAD;

		DirectX::XMMATRIX dxOffsetMat;
		DirectX::XMFLOAT3 dxOffset;
		dxOffsetMat = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

		float3 localOffset = ray.Look().rayOffset;

		DirectX::XMVECTOR local =
			DirectX::XMVectorSet(localOffset.x, localOffset.y, localOffset.z, 0.0f);

		DirectX::XMVECTOR world =
			DirectX::XMVector3TransformNormal(local, dxOffsetMat);

		DirectX::XMStoreFloat3(&dxOffset, world);

		float3 rayStartPos(
			pos.Look().x + dxOffset.x,
			pos.Look().y + dxOffset.y,
			pos.Look().z + dxOffset.z
		);

		float hitDistance = FLT_MAX;
		float3 hitPosCmp;
		float3 hitRotCmp;
		Entity otherEntity = it;

		// Rayの回転行列を作成
		DirectX::XMMATRIX rayRotMat = DirectX::XMMatrixRotationRollPitchYaw(
			rot.Look().pitch * RAD,
			rot.Look().yaw * RAD,
			rot.Look().roll * RAD);

		// Ray方向（World）
		DirectX::XMVECTOR rayDir = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		rayDir = DirectX::XMVector3TransformNormal(rayDir, rayRotMat);


		// 自分自身以外のcolliderを判定
		for (auto colliderIt : colliderEntities)
		{
			if (colliderIt == it) continue;

			// ColliderのComponentを取得
			const ComponentHandle<BoxCollider> collider = a_chunk.GetComponent<BoxCollider>(colliderIt);
			const ComponentHandle<Position> colliderPos = a_chunk.GetComponent<Position>(colliderIt);
			const ComponentHandle<Rotation> colliderRot = a_chunk.GetComponent<Rotation>(colliderIt);
			;

			// Colliderの回転行列を作成
			DirectX::XMMATRIX colliderRotMat = DirectX::XMMatrixRotationRollPitchYaw(
				colliderRot.Look().pitch * RAD,
				colliderRot.Look().yaw * RAD,
				colliderRot.Look().roll * RAD);

			// AABB half size
			DirectX::XMMATRIX invColliderRotMat = DirectX::XMMatrixTranspose(colliderRotMat); // ??]?s?????t = ?]?u

			// Ray方向をColliderローカルへ
			DirectX::XMVECTOR rayLocalDir = DirectX::XMVector3TransformNormal(rayDir, invColliderRotMat);

			// Colliderの座標を取得
			float3 colliderWorldPos = collider.Look().offset;
			colliderWorldPos.x += colliderPos.Look().x;
			colliderWorldPos.y += colliderPos.Look().y;
			colliderWorldPos.z += colliderPos.Look().z;

			// 相対位置に変換
			DirectX::XMVECTOR rayLocalPos = DirectX::XMVectorSet(
				rayStartPos.x - colliderWorldPos.x,
				rayStartPos.y - colliderWorldPos.y,
				rayStartPos.z - colliderWorldPos.z,
				1.0f);

			// 逆回転してColliderローカルへ
			rayLocalPos = DirectX::XMVector3TransformCoord(rayLocalPos, invColliderRotMat);

			DirectX::XMFLOAT3 rayLocalPosFloat;
			DirectX::XMStoreFloat3(&rayLocalPosFloat, rayLocalPos);

			// Rayの開始地点が埋まっていたらそのオブジェクトがヒットしたと判定しbreak
			if (
				abs(rayLocalPosFloat.x) <= collider.Look().collisionScale.x / 2 &&
				abs(rayLocalPosFloat.y) <= collider.Look().collisionScale.y / 2 &&
				abs(rayLocalPosFloat.z) <= collider.Look().collisionScale.z / 2)
			{
				hitDistance = 0.0f;
				hitPosCmp = rayStartPos;
				hitRotCmp = float3();
				break;
			}

			// どこにぶつかっているか判定を取る

			// どこにぶつかっているか判定を取る
			DirectX::XMFLOAT3 dir;
			DirectX::XMStoreFloat3(&dir, rayLocalDir);

			// AABB half size
			float3 half = collider.Look().collisionScale * 0.5f;

			// tの範囲
			float tMin = 0.0f;
			float tMax = FLT_MAX;

			// X軸
			if (abs(dir.x) < 1e-6f)
			{
				if (rayLocalPosFloat.x < -half.x || rayLocalPosFloat.x > half.x) continue;
			}
			else
			{
				float inv = 1.0f / dir.x;
				float t1 = (-half.x - rayLocalPosFloat.x) * inv;
				float t2 = (half.x - rayLocalPosFloat.x) * inv;
				if (t1 > t2) std::swap(t1, t2);
				tMin = std::max(tMin, t1);
				tMax = std::min(tMax, t2);
				if (tMin > tMax) continue;
			}

			// Y軸
			if (abs(dir.y) < 1e-6f)
			{
				if (rayLocalPosFloat.y < -half.y || rayLocalPosFloat.y > half.y) continue;
			}
			else
			{
				float inv = 1.0f / dir.y;
				float t1 = (-half.y - rayLocalPosFloat.y) * inv;
				float t2 = (half.y - rayLocalPosFloat.y) * inv;
				if (t1 > t2) std::swap(t1, t2);
				tMin = std::max(tMin, t1);
				tMax = std::min(tMax, t2);
				if (tMin > tMax) continue;
			}

			// Z軸
			if (abs(dir.z) < 1e-6f)
			{
				if (rayLocalPosFloat.z < -half.z || rayLocalPosFloat.z > half.z) continue;
			}
			else
			{
				float inv = 1.0f / dir.z;
				float t1 = (-half.z - rayLocalPosFloat.z) * inv;
				float t2 = (half.z - rayLocalPosFloat.z) * inv;
				if (t1 > t2) std::swap(t1, t2);
				tMin = std::max(tMin, t1);
				tMax = std::min(tMax, t2);
				if (tMin > tMax) continue;
			}

			// ここに来たらヒット（tMin が最初に当たる距離）
			float hitT = (tMin >= 0.0f) ? tMin : tMax;
			if (hitT < 0.0f) continue;

			// ローカルヒット位置
			DirectX::XMVECTOR localHit =
				DirectX::XMVectorSet(
					rayLocalPosFloat.x + dir.x * hitT,
					rayLocalPosFloat.y + dir.y * hitT,
					rayLocalPosFloat.z + dir.z * hitT,
					1.0f);

			// Worldに戻す
			DirectX::XMVECTOR worldHit =
				DirectX::XMVector3TransformCoord(localHit, colliderRotMat);
			worldHit = DirectX::XMVectorAdd(
				worldHit,
				DirectX::XMVectorSet(
					colliderWorldPos.x,
					colliderWorldPos.y,
					colliderWorldPos.z,
					1.0f));

			// ローカルのHit座標から当たった面を割り出す
			DirectX::XMFLOAT3 localHitFloat;
			DirectX::XMStoreFloat3(&localHitFloat, localHit);

			float3 hitSign(
				Sign(localHitFloat.x),
				Sign(localHitFloat.y),
				Sign(localHitFloat.z)
			);

			DirectX::XMFLOAT3 HitDis(collider.Look().collisionScale.x / 2 - std::abs(localHitFloat.x),
				collider.Look().collisionScale.y / 2 - std::abs(localHitFloat.y),
				collider.Look().collisionScale.z / 2 - std::abs(localHitFloat.z));

			float minDis;
			DirectX::XMFLOAT3 vec(0.0f, 0.0f, 0.0f);

			if (HitDis.x < HitDis.y)
			{
				minDis = HitDis.x;
				vec.x = hitSign.x;
			}
			else
			{
				minDis = HitDis.y;
				vec.y = hitSign.y;
			}

			if (HitDis.z < minDis)
			{
				minDis = HitDis.y;
				vec.x = 0.0f;
				vec.y = 0.0f;
				vec.z = hitSign.z;
			}

			//	当たった面と平行になる角度を出す

			float3 rotation;

			rotation.x = atan2(vec.y, vec.z) * DEG;
			rotation.y = atan2(vec.x, vec.z) * DEG;

			// ワールド回転に変換する

			float3 worldHitRotato(
				rotation.x + colliderRot.Look().pitch,
				rotation.y + colliderRot.Look().yaw,
				rotation.z + colliderRot.Look().roll
			);

			// ぶつかった距離を比較し、前回よりも近かったら当たった位置をWorld座標に戻して登録

			DirectX::XMFLOAT3 worldHitPosFloat;

			DirectX::XMStoreFloat3(&worldHitPosFloat, worldHit);

			float distance = sqrtf(
				powf(worldHitPosFloat.x - rayStartPos.x, 2)
				+ powf(worldHitPosFloat.y - rayStartPos.y, 2)
				+ powf(worldHitPosFloat.z - rayStartPos.z, 2)
			);

			if (distance < hitDistance)
			{
				hitDistance = distance;
				hitPosCmp.x = worldHitPosFloat.x;
				hitPosCmp.y = worldHitPosFloat.y;
				hitPosCmp.z = worldHitPosFloat.z;
				hitRotCmp = worldHitRotato;
				otherEntity = colliderIt;
			}
		}
		if (hitDistance == FLT_MAX)
		{
			continue;
		}

		a_chunk.AddComponent(it, RayInfomation(it, otherEntity, hitPosCmp, hitRotCmp));

	}
}

