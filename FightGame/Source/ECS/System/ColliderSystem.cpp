#include "ColliderSystem.h"
#include "Components.h"
#include "SystemAssist.h"

using namespace Component;

#include <math.h>
struct SystemHitResult;

bool IsOBBHitObb(const OBBCollider& thisCollider, const OBBCollider& otherCollider, SystemHitResult& outThisResult, SystemHitResult& outOtherResult);

bool IsOBBHit(const ComponentHandle<OBBCollider>& thisCollider, const ComponentHandle<OBBCollider>& otherCollider, SystemHitResult& outThisResult, SystemHitResult& outOtherResult)
{
	return IsOBBHitObb(thisCollider.Look(), otherCollider.Look(), outThisResult, outOtherResult);
}

bool SweepOBBTranslate(
	const ComponentHandle<OBBCollider>& a_movingCollider,
	float3 a_motion,
	const ComponentHandle<OBBCollider>& a_staticCollider,
	float& outHitTime,
	float3& outNormal,
	float& outDepth);

// 当たり判定結果を入れるための戻り値、コンポーネントではないので注意
struct SystemHitResult
{
	bool isHit;
	bool isTrigger;
	float3 normal;
	float depth;
	InfoColliderType otherType;

	SystemHitResult(bool a_isHit, bool a_isTrigger, float3 a_normal, float a_depth, InfoColliderType a_otherType)
		: isHit(a_isHit)
		, isTrigger(a_isTrigger)
		, normal(a_normal)
		, depth(a_depth)
		, otherType(a_otherType)
	{
	}

	SystemHitResult()
		: isHit(false)
		, isTrigger(false)
		, normal()
		, depth(0.0f)
		, otherType(NONE_INFO)
	{
	}
};


void ColliderSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<BoxCollider, OBBCollider, Scale>>();

	// TODO	Offsetをデバッグ描画にも適用する

	for (auto it : view)
	{
		const ComponentHandle<BoxCollider> collider = a_chunk.GetComponent<BoxCollider>(it);
		ComponentHandle<OBBCollider> obbCollider = a_chunk.GetComponent<OBBCollider>(it);
		const ComponentHandle<Scale> scale = a_chunk.GetComponent<Scale>(it);

		// !!!New!!! Pose 優先の座標・回転
		float3 floatPosition = GetEntityPosePos(a_chunk, it);
		float3 floatRotation = GetEntityPoseRot(a_chunk, it);
		const ComponentHandle<Position> position = a_chunk.GetComponent<Position>(it);
		const ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(it);
		const ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(it);
		if (!pose.IsValid() && (!position.IsValid() || !rotation.IsValid())) continue;
		floatPosition += collider.Look().offset;

		std::vector<float3> vertices;


		for (int i = 0; i < 8; i++)
		{
			float directionX = 1.0f;
			float directionY = 1.0f;
			float directionZ = 1.0f;

			directionX = (i) % 2 == 0 ? 1.0f : -1.0f;
			directionY = (i / 2) % 2 == 0 ? 1.0f : -1.0f;
			directionZ = (i / 4) % 2 == 0 ? 1.0f : -1.0f;

			float3 vertexPos = {
				collider.Look().collisionScale.x * directionX * 0.5f,
				collider.Look().collisionScale.y * directionY * 0.5f,
				collider.Look().collisionScale.z * directionZ * 0.5f };

			vertices.push_back(vertexPos);
		}



		Matrix3X3 matrix = CollectMatrix(vertices);
		Matrix3X3 eigenVectors = GetEigenVectors(matrix);

		float3 vec1, vec2, vec3;
		{
			float x = eigenVectors[0][0];
			float y = eigenVectors[0][1];
			float z = eigenVectors[0][2];

			vec1 = float3{ x, y, z };
		}
		{
			float x = eigenVectors[1][0];
			float y = eigenVectors[1][1];
			float z = eigenVectors[1][2];

			vec2 = float3{ x, y, z };
		}
		{
			float x = eigenVectors[2][0];
			float y = eigenVectors[2][1];
			float z = eigenVectors[2][2];

			vec3 = float3{ x, y, z };
		}


		vec1 = Normalize(vec1);
		vec2 = Normalize(vec2);
		vec3 = Normalize(vec3);

		// 全頂点に対して内積を取り、最小値・最大値を計算する
		float min1 = std::numeric_limits<float>::max();
		float min2 = std::numeric_limits<float>::max();
		float min3 = std::numeric_limits<float>::max();
		float max1 = std::numeric_limits<float>::lowest();
		float max2 = std::numeric_limits<float>::lowest();
		float max3 = std::numeric_limits<float>::lowest();

		for (int i = 0; i < vertices.size(); i++)
		{
			float3 pos = vertices[i];
			float dot1 = DotFloat3(vec1, pos);
			if (dot1 > max1)
			{
				max1 = dot1;
			}
			if (dot1 < min1)
			{
				min1 = dot1;
			}

			float dot2 = DotFloat3(vec2, pos);
			if (dot2 > max2)
			{
				max2 = dot2;
			}
			if (dot2 < min2)
			{
				min2 = dot2;
			}

			float dot3 = DotFloat3(vec3, pos);
			if (dot3 > max3)
			{
				max3 = dot3;
			}
			if (dot3 < min3)
			{
				min3 = dot3;
			}
		}

		float len1 = max1 - min1;
		float len2 = max2 - min2;
		float len3 = max3 - min3;


		// ローカル座標に変換

		float pitchFixed = (obbCollider.Look().obbBitFlag & OBB_FIXED_PITCH ? 0.0f : 1.0f);
		float yawFixed = (obbCollider.Look().obbBitFlag & OBB_FIXED_YAW ? 0.0f : 1.0f);
		float rollFixed = (obbCollider.Look().obbBitFlag & OBB_FIXED_ROLL ? 0.0f : 1.0f);

		float3 floatScale = { scale.Look().x, scale.Look().y, scale.Look().z };
		float3 appliedRotation = {
			floatRotation.x * pitchFixed,
			floatRotation.y * yawFixed,
			floatRotation.z * rollFixed
		};

		RigidTransform transform(floatPosition, appliedRotation, floatScale);

		float3 edge1 = transform.MultiplyVector(vec1 * len1);
		float3 edge2 = transform.MultiplyVector(vec2 * len2);
		float3 edge3 = transform.MultiplyVector(vec3 * len3);


		std::array<float3, 3> edge =
		{
			edge1, edge2, edge3,
		};

		obbCollider->edges = edge;

		float3 center1 = (vec1 * (max1 + min1)) * 0.5f;
		float3 center2 = (vec2 * (max2 + min2)) * 0.5f;
		float3 center3 = (vec3 * (max3 + min3)) * 0.5f;

		float3 addedCenter =
		{
			center1.x + center2.x + center3.x,
			center1.y + center2.y + center3.y,
			center1.z + center2.z + center3.z,
		};

		float3 orign = transform.MultiplyPoint(addedCenter);

		// !!!New!!!
		if (obbCollider.Look().setOldCenter)
		{
			obbCollider->oldCenter = obbCollider.Look().center;
		}
		else
		{
			obbCollider->oldCenter = orign;
			obbCollider->setOldCenter = true;
		}
		obbCollider->center = orign;

		obbCollider->axis[0] = Normalize(transform.MultiplyVector(vec1));
		obbCollider->axis[1] = Normalize(transform.MultiplyVector(vec2));
		obbCollider->axis[2] = Normalize(transform.MultiplyVector(vec3));

		obbCollider->half = float3(len1 / 2, len2 / 2, len3 / 2);

	}
}

void ColliderCheckSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// 一度のみviewを作る
	ComponentView view = a_chunk.GetView<ComponentTypes<OBBCollider>>();


	// 無駄にview作成や走査しないために一度EntityとColliderをセットで確保
	using OBBEntry = std::pair<Entity, ComponentHandle<OBBCollider>>;

	std::vector<OBBEntry> obbEntries;
	for (Entity it : view)
	{
		obbEntries.push_back(OBBEntry(it, a_chunk.GetComponent<OBBCollider>(it)));
	}


	// 走査する
	for (auto thisIt = obbEntries.begin();
		thisIt != obbEntries.end();
		++thisIt)
	{
		auto& [thisEntity, thisCollider] = *thisIt;
		auto hitInfoThis = a_chunk.GetComponent<HitInfomation>(thisEntity);


		for (auto otherIt = thisIt + 1;
			otherIt != obbEntries.end();
			++otherIt)
		{
			auto& [otherEntity, otherCollider] = *otherIt;

			SystemHitResult thisResult;
			SystemHitResult otherResult;

			// 当たっているか確認
			if (IsOBBHit(thisCollider, otherCollider, thisResult, otherResult))
			{
				if (hitInfoThis.IsValid())
				{
					if (thisResult.isHit)
					{
						hitInfoThis->hitResults.push_back(HitInfomation::HitResult(otherEntity, false, thisResult.normal, thisResult.depth, float3(), 0.0f, thisResult.otherType));
					}
					else if (thisResult.isTrigger)
					{
						hitInfoThis->triggerResults.push_back(HitInfomation::TriggerResult(otherEntity));
					}
				}

				auto hitInfoOther = a_chunk.GetComponent<HitInfomation>(otherEntity);
				if (hitInfoOther.IsValid())
				{
					if (otherResult.isHit)
					{
						hitInfoOther->hitResults.push_back(HitInfomation::HitResult(thisEntity, false, otherResult.normal, otherResult.depth, float3(), 0.0f, otherResult.otherType));
					}
					else if (otherResult.isTrigger)
					{
						hitInfoOther->triggerResults.push_back(HitInfomation::TriggerResult(thisEntity));
					}
				}

				continue;
			}

			//// !!!New!!!
			//const bool thisLocked = (thisCollider.Look().obbBitFlag & OBB_PushOutLocked) != 0;
			//const bool otherLocked = (otherCollider.Look().obbBitFlag & OBB_PushOutLocked) != 0;
			//if (thisLocked == otherLocked) continue;
			//if ((thisCollider.Look().obbBitFlag & OBB_TRIGGER) || (otherCollider.Look().obbBitFlag & OBB_TRIGGER)) continue;

			//const ComponentHandle<OBBCollider> movingCollider = otherLocked ? thisCollider : otherCollider;
			//const ComponentHandle<OBBCollider> staticCollider = otherLocked ? otherCollider : thisCollider;
			//const Entity staticEntity = otherLocked ? otherEntity : thisEntity;
			//auto hitInfoMoving = otherLocked ? hitInfoThis : a_chunk.GetComponent<HitInfomation>(otherEntity);
			//if (!hitInfoMoving.IsValid()) continue;
			//if (!movingCollider.Look().setOldCenter) continue;

			//float3 motion =
			//{
			//	movingCollider.Look().center.x - movingCollider.Look().oldCenter.x,
			//	movingCollider.Look().center.y - movingCollider.Look().oldCenter.y,
			//	movingCollider.Look().center.z - movingCollider.Look().oldCenter.z,
			//};

			//float hitTime = 0.0f;
			//float3 sweepNormal;
			//float sweepDepth = 0.0f;
			//if (!SweepOBBTranslate(movingCollider, motion, staticCollider, hitTime, sweepNormal, sweepDepth)) continue;

			//hitInfoMoving->hitResults.push_back(HitInfomation::HitResult(staticEntity, true, sweepNormal, sweepDepth, motion, hitTime, PUSHLOCKED));
		}
	}

}

bool SweepOBBTranslate(
	const ComponentHandle<OBBCollider>& a_movingCollider,
	float3 a_motion,
	const ComponentHandle<OBBCollider>& a_staticCollider,
	float& outHitTime,
	float3& outNormal,
	float& outDepth)
{
	// 移動量が十分小さければ false
	float motionAmount = NormalizeLength(a_motion.x, a_motion.y, a_motion.z);
    if(motionAmount < 0.001f)
	{
		return false;
	}
	// 移動線分と静止 OBB の初接触 t (0?1) を求める
	const float3 startCenter = a_movingCollider.Look().oldCenter;
	OBBCollider movingAtT = a_movingCollider.Look();
	const OBBCollider staticObb = a_staticCollider.Look();

	
	static constexpr int kSweepSteps = 32;
	int sweepSteps = std::max(kSweepSteps * (int)(0.5f + motionAmount * 0.3f), 1000);

	for (int step = 1; step <= sweepSteps; ++step)
	{
		const float t = static_cast<float>(step) / static_cast<float>(sweepSteps);
		movingAtT.center =
		{
			startCenter.x + a_motion.x * t,
			startCenter.y + a_motion.y * t,
			startCenter.z + a_motion.z * t,
		};

		SystemHitResult movingResult;
		SystemHitResult staticResult;
		if (!IsOBBHitObb(movingAtT, staticObb, movingResult, staticResult)) continue;
		if (!movingResult.isHit) continue;

		outHitTime = t;
		outNormal = movingResult.normal;
 		outDepth = (1.0f - t) * motionAmount;
		return true;
	}

	// 接触しなければ false
	return false;
}

void ColliderBackSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<HitInfomation, OBBCollider, MotionResult>>();


	for (auto it : view)
	{
		const ComponentHandle<HitInfomation> info = a_chunk.GetComponent<HitInfomation>(it);
		const ComponentHandle<OBBCollider> collider = a_chunk.GetComponent<OBBCollider>(it);
		ComponentHandle<MotionResult> result = a_chunk.GetComponent<MotionResult>(it);
		ComponentHandle<Velocity> velocity = a_chunk.GetComponent<Velocity>(it);
		if (info.Look().hitResults.size() >= 2)
		{
			int breakP;
			breakP = (int)info.Look().hitResults.size();
		}

		for (const auto& hitInfoIt : info.Look().hitResults)
		{
			if (collider.Look().obbBitFlag & OBB_TRIGGER || collider.Look().obbBitFlag & OBB_PushOutLocked) continue;

			switch (hitInfoIt.otherType)
			{
			case DEFAULT:
				// 中で変数を作りたいためスコープで囲う
			{
				float half = hitInfoIt.depth * 0.5f;
				result->posOffset.x += hitInfoIt.normal.x * half;
				result->posOffset.y += hitInfoIt.normal.y * half;
				result->posOffset.z += hitInfoIt.normal.z * half;
			}
			break;

			case PUSHLOCKED:
				if(hitInfoIt.isSweep)
				{
					float rate = 1.0f - hitInfoIt.normalizeHitTime;
					result->posOffset.x -= hitInfoIt.sweepMotion.x * rate;
					result->posOffset.y -= hitInfoIt.sweepMotion.y * rate;
					result->posOffset.z -= hitInfoIt.sweepMotion.z * rate;
					DebugConsole::SetDrawPos(1, 25);
					static int count = 0;
					count++;

					std::cout << "Count = " << count << std::endl;
				}
				else
				{
					result->posOffset.x += hitInfoIt.normal.x * hitInfoIt.depth;
					result->posOffset.y += hitInfoIt.normal.y * hitInfoIt.depth;
					result->posOffset.z += hitInfoIt.normal.z * hitInfoIt.depth;
				}

				break;
			}

			if (!velocity.IsValid()) continue;

			// 法線方向に入っている速度成分を打ち切る
			float3 vel(velocity.Look().x, velocity.Look().y, velocity.Look().z);
			
		    // Velocityと法線方向の一致度を内積で取得
			float intoSurface = DotFloat3(vel, hitInfoIt.normal);
			if (intoSurface >= 0.0f) continue;
			// 一致度に応じて減らす
			velocity->x -= hitInfoIt.normal.x * intoSurface * 1.0f;
			velocity->y -= hitInfoIt.normal.y * intoSurface * 1.0f;
			velocity->z -= hitInfoIt.normal.z * intoSurface * 1.0f;
		}

	}
}


void SectorCheckSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// HitInfomationを保持したEntityを予め取得し保持
	ComponentView targetView = a_chunk.GetView<ComponentTypes<HitInfomation>>();

	using TargetEntry = std::pair<Entity, ComponentHandle<HitInfomation>>;
	std::vector<TargetEntry> targetEntries;
	for (Entity it : targetView)
	{
		targetEntries.push_back(TargetEntry(it, a_chunk.GetComponent<HitInfomation>(it)));
	}

	// 扇形当たり判定を持つEntityのViewを取得
	ComponentView sectorView = a_chunk.GetView<ComponentTypes<SectorHitJudge>>();

	// ループ開始
	for (auto sectorIt : sectorView)
	{
		const ComponentHandle<SectorHitJudge> sector = a_chunk.GetComponent<SectorHitJudge>(sectorIt);
		float3 sectorPos = GetEntityPosePos(a_chunk, sectorIt);
		float3 sectorRot = GetEntityPoseRot(a_chunk, sectorIt);

		
		float3 forward = GetForward(float3(0.0f, sectorRot.y, 0.0f));
		// 内角の戻り値に合わせて角度をcosの半角に変換する
		float cosHalfAngle = cosf(sector.Look().angle * 0.5f * RAD);

		// HitInfomationを保持した対象をループで回す
		for (auto& [targetEntity, targetInfo] : targetEntries)
		{
			float3 targetPos = GetEntityPosePos(a_chunk, targetEntity);

			// 敵と自身の距離を取得し範囲内か判定する
			float3 difference = float3(
				targetPos.x - sectorPos.x,
				0.0f,
				targetPos.z - sectorPos.z);
			float distance = NormalizeLength(difference.x, difference.y, difference.z);
			if (distance < sector.Look().minLength || distance > sector.Look().maxLength) continue;

			// AngleをHalfAngleに変換し内角で判定を取る
			if (distance > 1e-5f)
			{
				float cosTheta = DotFloat3(forward, Normalize(difference));
				if (cosTheta < cosHalfAngle) continue;
			}

			// 高さを比較する
			float differenceY = targetPos.y - sectorPos.y;
			if (differenceY > sector.Look().maxHeight || differenceY < -sector.Look().maxLowness) continue;

			// HitInfomationに結果を代入する
			targetInfo->triggerResults.push_back(HitInfomation::TriggerResult(sectorIt));
		}
	}

}


bool IsOBBHitObb(const OBBCollider& thisCollider, const OBBCollider& otherCollider, SystemHitResult& outThisResult, SystemHitResult& outOtherResult)
{
	float3 differenceVector =
	{
		otherCollider.center.x - thisCollider.center.x,
		otherCollider.center.y - thisCollider.center.y,
		otherCollider.center.z - thisCollider.center.z,
	};

	Matrix3X3 R;
	Matrix3X3 AbsR;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = DotFloat3(thisCollider.axis[i], otherCollider.axis[j]);
			AbsR[i][j] = abs(R[i][j]) + 0.0001f;
		}
	}


	// A 側の射影半径
	float rThisHalf[3] = { thisCollider.half.x, thisCollider.half.y, thisCollider.half.z };

	float resultDepth = FLT_MAX;
	float3 resultNormal;

	// A0, A1, A2
	for (int i = 0; i < 3; i++)
	{
		// B 側の射影半径
		float rOther =
			otherCollider.half.x * AbsR[i][0] +
			otherCollider.half.y * AbsR[i][1] +
			otherCollider.half.z * AbsR[i][2];

		// 中心差の射影
		float t = std::fabs(DotFloat3(differenceVector, thisCollider.axis[i]));

		// 分離していたら抜ける
		float overlap = (rThisHalf[i] + rOther) - t;
		if (overlap < 0.0f)
		{
			outThisResult.isHit = false;
			outThisResult.isTrigger = false;
			outThisResult.normal = float3();
			outThisResult.depth = 0.0f;
			outOtherResult.isHit = false;
			outOtherResult.isTrigger = false;
			outOtherResult.normal = float3();
			outOtherResult.depth = 0.0f;
			return false;
		}

		if (overlap < resultDepth)
		{
			resultDepth = overlap;
			//resultNormal = thisCollider.axis[i];
			float3 axis = thisCollider.axis[i];
			// 中心差と逆向きなら反転
			if (DotFloat3(differenceVector, axis) > 0.0f)
			{
				axis.x = -axis.x;
				axis.y = -axis.y;
				axis.z = -axis.z;
			}
			resultNormal = axis;
		}
	}

	// B 側の射影半径
	float rOtherHalf[3] = { otherCollider.half.x, otherCollider.half.y, otherCollider.half.z };

	// B0, B1, B2
	for (int j = 0; j < 3; j++)
	{
		float rThis =
			thisCollider.half.x * AbsR[0][j] +
			thisCollider.half.y * AbsR[1][j] +
			thisCollider.half.z * AbsR[2][j];

		float t = std::fabs(DotFloat3(differenceVector, otherCollider.axis[j]));

		float overlap = (rOtherHalf[j] + rThis) - t;
		if (overlap < 0.0f)
		{
			outThisResult.isHit = false;
			outThisResult.normal = float3();
			outThisResult.depth = 0.0f;
			outOtherResult.isHit = false;
			outOtherResult.normal = float3();
			outOtherResult.depth = 0.0f;
			return false;
		}

		if (overlap < resultDepth)
		{
			resultDepth = overlap;

			//resultNormal = otherCollider.axis[j];
			float3 axis = otherCollider.axis[j];

			if (DotFloat3(differenceVector, axis) > 0.0f)
			{
				axis.x = -axis.x;
				axis.y = -axis.y;
				axis.z = -axis.z;
			}

			resultNormal = axis;

		}

	}

	// A0, A1, A2
	for (int i = 0; i < 3; i++)
	{
		int i1 = (i + 1) % 3;
		int i2 = (i + 2) % 3;

		for (int j = 0; j < 3; j++)
		{
			int j1 = (j + 1) % 3;
			int j2 = (j + 2) % 3;

			float t = std::fabs(
				DotFloat3(differenceVector, thisCollider.axis[i2]) * R[i1][j] -
				DotFloat3(differenceVector, thisCollider.axis[i1]) * R[i2][j]
			);

			float rThis =
				rThisHalf[i1] * AbsR[i2][j] +
				rThisHalf[i2] * AbsR[i1][j];

			float rOther =
				rOtherHalf[j1] * AbsR[i][j2] +
				rOtherHalf[j2] * AbsR[i][j1];

			float overlap = (rThis + rOther) - t;
			if (overlap < 0.0f)
			{
				outThisResult.isHit = false;
				outThisResult.normal = float3();
				outThisResult.depth = 0.0f;
				outOtherResult.isHit = false;
				outOtherResult.normal = float3();
				outOtherResult.depth = 0.0f;
				return false;
			}

		}
	}


	// Triggerだった場合Trigger情報をResultに渡して返す
	if ((thisCollider.obbBitFlag & OBB_TRIGGER) || (otherCollider.obbBitFlag & OBB_TRIGGER))
	{
		outThisResult.isTrigger = true;
		outOtherResult.isTrigger = true;
		outThisResult.normal = float3();
		outOtherResult.normal = float3();
		outThisResult.depth = 0.0f;
		outOtherResult.depth = 0.0f;
		return true;
	}

	float3 otherResultNormal(-resultNormal.x, -resultNormal.y, -resultNormal.z);
	// TriggerではないのでCollider情報を返す
	outThisResult.isHit = true;
	outOtherResult.isHit = true;
	outThisResult.normal = resultNormal;
	outOtherResult.normal = otherResultNormal;
	outThisResult.depth = resultDepth;
	outOtherResult.depth = resultDepth;

	// 移動出来るかを取る
	bool isThisMove = !(thisCollider.obbBitFlag & OBB_PushOutLocked);
	bool isOtherMove = !(otherCollider.obbBitFlag & OBB_PushOutLocked);
	// 自分側の設定を相手側のResultに入力
	if (isThisMove)
	{
		outOtherResult.otherType = DEFAULT;
	}
	else
	{
		outOtherResult.otherType = PUSHLOCKED;
	}
	// 相手側の設定を自分側のResultに入力
	if (isOtherMove)
	{
		outThisResult.otherType = DEFAULT;
	}
	else
	{
		outThisResult.otherType = PUSHLOCKED;
	}

	return true;

}


