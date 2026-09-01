#include "TransformSystem.h"
#include "Components.h"
#include "SystemAssist.h"

using namespace Component;

float kNoiseXTimeOffsert = 0.0f;
float kNoiseYTimeOffsert = 371.52f;
float kNoiseZTimeOffsert = 391.38f;

void ApplyPoseToTransform(Chunk& a_chunk);
void SetPosePos(Entity a_entity, Chunk& a_chunk);
void SetPoseRot(Entity a_entity, Chunk& a_chunk);
void SetCurrentPosePos(Entity a_entity, Chunk& a_chunk);
void SetCurrentPoseRot(Entity a_entity, Chunk& a_chunk);
bool IsSetFixedPos(const ComponentHandle<PosePosState>& a_posePosState, const std::vector<PosePosStateEnum>& a_setPoseState);
bool IsSetFixedRot(const ComponentHandle<PoseRotState>& a_posePosState, const std::vector<PoseRotStateEnum>& a_setPoseState);
float3 GetLookRot(Chunk& a_chunk, const Entity user, const Entity target);

void InputMoveSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// Poseを変化させるEntityを取得
	ComponentView moveView = a_chunk.GetView<ComponentTypes<InputMove, MoveInputResult, MotionResult>>();

	// カメラを取得
	Entity cameraEntity = GetCamera(a_chunk);


	// 結果を返す
	for (auto it : moveView)
	{
		if (!IsActionAllowed(a_chunk, it, ActionFlag_Move)) continue;

		// 入力を取得
		ComponentHandle<MoveInputResult> inputResult = a_chunk.GetComponent<MoveInputResult>(it);

		// 座標を取得
		ComponentHandle<MotionResult> motionResult = a_chunk.GetComponent<MotionResult>(it);
		// 移動速度などの情報を変更しない形で取得
		ComponentHandle<InputMove> inputMove = a_chunk.GetComponent<InputMove>(it);

		// 左スティックの移動値を取得し、移動していた場合の未処理
		if (!inputResult.Look().isInput) continue;

		
		// 移動を求めるために行列を作成
		DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();

		// 角度を持っている場合は向いてる方向に進むようにする
		ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(cameraEntity);
		if (rotation.IsValid())
		{
			// 横回転のみをかける
			matrix *= DirectX::XMMatrixRotationRollPitchYaw(0.0f, rotation.Look().yaw * RAD, 0.0f);
		}

		//// カメラの横回転からみた前方向を取得する
		//DirectX::XMVECTOR forwardVector = DirectX::XMVector3TransformNormal({ 0,0,1 }, matrix);
		//// 前方向の入力にベクトルをかける
		//DirectX::XMVECTOR moveForwardVector = DirectX::XMVectorScale(forwardVector, leftAxis.y * (1 + leftAxis.magnitube / 2) * inputMove.Look().moveSpeed.y * a_context.deltaTime);

		//DirectX::XMVECTOR besideVector = DirectX::XMVector3TransformNormal({ 1,0,0 }, matrix);
		//DirectX::XMVECTOR moveBesideVector = DirectX::XMVectorScale(besideVector, leftAxis.x * (1 + leftAxis.magnitube / 2) * inputMove.Look().moveSpeed.x * a_context.deltaTime);

		//DirectX::XMFLOAT3 moveForward;
		//DirectX::XMStoreFloat3(&moveForward, moveForwardVector);

		//DirectX::XMFLOAT3 moveBeside;
		//DirectX::XMStoreFloat3(&moveBeside, moveBesideVector);

		//float3 move = { moveForward.x + moveBeside.x, moveForward.y + moveBeside.y, moveForward.z + moveBeside.z };

		float3 move =
		{
			inputResult.Look().moveDir.x * (1 + inputResult.Look().magnitube / 2) * inputMove.Look().moveSpeed.x * a_context.deltaTime,
			0.0f,
			inputResult.Look().moveDir.y * (1 + inputResult.Look().magnitube / 2) * inputMove.Look().moveSpeed.y * a_context.deltaTime
		};
		motionResult->posOffset.x += move.x;
		motionResult->posOffset.y += move.y;
		motionResult->posOffset.z += move.z;

		//ComponentHandle<Force> force = a_chunk.GetComponent<Force>(it);
		//if (!force.IsValid()) continue;
		//force->force.x = move.x / a_context.deltaTime * 0.4f;
		//force->force.z = move.z / a_context.deltaTime * 0.4f;
		//force->attenuation = 0.8f;


		// 攻撃中だった場合攻撃をキャンセルする
		CancelPlayerAttackIfAble(a_chunk, it);


	}
}


void LookMoveSystem(Chunk& a_chunk, const SystemContext& a_context)
{

	Entity cameraEntity = GetCamera(a_chunk);
	ComponentHandle<Rotation> cameraRot = a_chunk.GetComponent<Rotation>(cameraEntity);

		
	ComponentView view = a_chunk.GetView<ComponentTypes<LookMove, MoveInputResult, Pose, MotionResult>>();

	for (auto it : view)
	{
		// 入力結果を取得
		ComponentHandle<MoveInputResult> inputResult = a_chunk.GetComponent<MoveInputResult>(it);

		// 入力されていなかったら抜ける
		if (!inputResult.Look().isInput) continue;

		// 移動不可能だった場合抜ける
		if (!IsActionAllowed(a_chunk, it, ActionFlag_Move)) continue;
		if (!IsActionAllowed(a_chunk, it, ActionFlag_LookMove)) continue;
		if (!IsActionAllowed(a_chunk, it, ActionFlag_RotChange)) continue;

		// 角度に変換
		float targetAngle = atan2(inputResult.Look().moveDir.x, inputResult.Look().moveDir.y) * DEG;

		// 現在角度を取得
		ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(it);
		float currentAngle = fmod(pose.Look().rot.y + 180.0f, 360.0f) - 180.0f;
		while (currentAngle >  180.0f) currentAngle -= 360.0f;
		while (currentAngle < -180.0f) currentAngle += 360.0f;

		// 角度差から回転方向を取得
		float angleDiff = targetAngle - currentAngle;
		while (angleDiff >  180.0f) angleDiff -= 360.0f;
		while (angleDiff < -180.0f) angleDiff += 360.0f;

		float sign = std::abs(angleDiff) < 180.0f ?  Sign(angleDiff) : Sign(angleDiff) * -1.0f;
		// 速度を取得する
		ComponentHandle<LookMove> move = a_chunk.GetComponent<LookMove>(it);
		float speed = move.Look().rotateSpeed  * a_context.deltaTime < std::abs(angleDiff) ? move.Look().rotateSpeed  * a_context.deltaTime : std::abs(angleDiff);
		// リザルトに書き込み
		ComponentHandle<MotionResult> result = a_chunk.GetComponent<MotionResult>(it);
		result->rotOffset.y += sign * speed;
	}

}

void LookOnSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView actionView = a_chunk.GetView<ComponentTypes<LookOnAction, FixedResult>>();

	Entity cameraTarget = kInvalidEntity;
	bool usePlayer = false;
	bool useFinelRot = false;
	float3 finalRot;
	for (auto it : actionView)
	{
		bool isPlayer = a_chunk.GetComponent<PlayerTag>(it).IsValid();

		ComponentHandle<LookOnAction> look = a_chunk.GetComponent<LookOnAction>(it);
		ComponentHandle<FixedResult> lookResult = a_chunk.GetComponent<FixedResult>(it);
		if (!IsActionAllowed(a_chunk, it, ActionFlag_RotChange))
		{
			lookResult->newRot = look.Look().finalRot;
			cameraTarget = look.Look().target;
			usePlayer = true;
			useFinelRot = true;
			finalRot = look.Look().finalRot;
			continue;
		};
		float3 finalRot = GetLookRot(a_chunk, it, look.Look().target);

		lookResult->newRot.x = 0.0f;
		lookResult->newRot.y = finalRot.y;
		lookResult->newRot.z = 0.0f;

		look->finalRot = lookResult.Look().newRot;

		if (isPlayer)
		{
			cameraTarget = look.Look().target;
			usePlayer = true;	
		}
	}

	// Playerがロックオンした場合カメラも移動する
	if (!usePlayer) return;

	ComponentView rigView = a_chunk.GetView<ComponentTypes<CameraRigTag, RigLookOn, MotionResult>>();
	for (auto it : rigView)
	{
		float3 targetRot;
		if (useFinelRot) targetRot = finalRot;
		else targetRot = GetLookRot(a_chunk, it, cameraTarget);
		
		float3 currentRot = GetEntityPoseRot(a_chunk, it);
		float delta = DeltaDeg(currentRot.y, targetRot.y);
		float speed = 2.0f * a_context.deltaTime;
		float move;
		if (speed > std::abs(delta)) move = delta;
		else move = speed * Sign(delta);

		ComponentHandle<MotionResult> cameraResult = a_chunk.GetComponent<MotionResult>(it);
		cameraResult->rotOffset.y += move;
	}
}


void InputRotatoSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// 回転入力のEntityを取得
	ComponentView rotatoView = a_chunk.GetView<ComponentTypes<InputRotato, MotionResult>>();

	// 入力を取得
	// コントローラー入力されているか
	// 入力を取得
	// コントローラー入力されているか
	bool isMagnitube = a_context.input.GetRightAxis().magnitube;
	// 
	float rx = isMagnitube ? a_context.input.GetRightAxis().x : a_context.input.GetMouseAxis().x;
	float ry = isMagnitube ? a_context.input.GetRightAxis().y : a_context.input.GetMouseAxis().y;
	// Rotato回転を処理
	for (auto it : rotatoView)
	{
		if (!IsActionAllowed(a_chunk, it, ActionFlag_Aim)) continue;

		const ComponentHandle<InputRotato> inputRotato = a_chunk.GetComponent<InputRotato>(it);
		ComponentHandle<MotionResult> result = a_chunk.GetComponent<MotionResult>(it);

		
		// コントローラー入力を優先
		if (isMagnitube)
		{
			// 速度と補正に応じて回転させる
			float rightMagnitube = a_context.input.GetRightAxis().magnitube;
			//inputRotatoResult->rotatoOffset.y = rx * (1 + rightMagnitube / 2) * a_context.deltaTime * inputRotato.Look().rotatoSpeed.x;
			//inputRotatoResult->rotatoOffset.x = ry * (1 + rightMagnitube / 2) * a_context.deltaTime * inputRotato.Look().rotatoSpeed.y * -1.0f;

			result->rotOffset.y += rx * rightMagnitube * a_context.deltaTime * inputRotato.Look().padRotatoSpeed.x;
			result->rotOffset.x += ry * rightMagnitube * a_context.deltaTime * inputRotato.Look().padRotatoSpeed.y * -1.0f;
		}
		else // コントローラー入力以外(マウス)の回転
		{
			result->rotOffset.x += (ry / 7.0f) * a_context.deltaTime * inputRotato.Look().mouseRotatoSpeed.y;
			result->rotOffset.y += (rx / 7.0f) * a_context.deltaTime * inputRotato.Look().mouseRotatoSpeed.x;
		}
	}
}


void FlipSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<FlipComponent, MotionResult, HitInfomation>>();

	for (auto it : view)
	{
		const ComponentHandle<HitInfomation> info = a_chunk.GetComponent<HitInfomation>(it);
		ComponentHandle<MotionResult> flipResult = a_chunk.GetComponent<MotionResult>(it);
		if (!info.Look().hitResults.empty() || !info.Look().triggerResults.empty())
		{
			flipResult->rotOffset.x += 180.0f;
		}

	}
}

void FollowTransformSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// 座標のView
	ComponentView followPosView = a_chunk.GetView<ComponentTypes<FollowPosition, FixedResult>>();
	// 座標の
	for (auto it : followPosView)
	{
		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(it);
		const ComponentHandle<FollowPosition> followData = a_chunk.GetComponent<FollowPosition>(it);

		DirectX::XMMATRIX rot = DirectX::XMMatrixIdentity();
		const ComponentHandle<Pose> targetPose = a_chunk.GetComponent<Pose>(followData.Look().targetEntity);
		if (!targetPose.IsValid()) continue;

		// Offset
		float3 worldOffset;

		// Targetの向いている方向から見たOffsetを計算する処理
		if (targetPose.IsValid() && followData.Look().flag & FOLLOW_POS_LOCALOFFSET)
		{
			float3 targetRot = targetPose.Look().rot;
			// 行列を使って注視点を求める
			float pitch = targetRot.x * RAD;
			float yaw = targetRot.y * RAD;
			float roll = targetRot.z * RAD;

			rot = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			float3 localOffset = followData.Look().positionOffset;

			DirectX::XMVECTOR local =
				DirectX::XMVectorSet(localOffset.x, localOffset.y, localOffset.z, 0.0f);

			DirectX::XMVECTOR world =
				DirectX::XMVector3TransformNormal(local, rot);

			DirectX::XMFLOAT3 dxWorldOffset;
			DirectX::XMStoreFloat3(&dxWorldOffset, world);

			worldOffset.x = dxWorldOffset.x;
			worldOffset.y = dxWorldOffset.y;
			worldOffset.z = dxWorldOffset.z;
		}
		else
		{
			float3 offset = followData.Look().positionOffset;
			worldOffset.x = offset.x;
			worldOffset.y = offset.y;
			worldOffset.z = offset.z;
		}


		float3 pos(0.0f, 0.0f, 0.0f);
		if (!(followData.Look().flag & FOLLOW_POS_FIXED_X)) pos.x = targetPose.Look().pos.x;
		if (!(followData.Look().flag & FOLLOW_POS_FIXED_Y)) pos.y = targetPose.Look().pos.y;
		if (!(followData.Look().flag & FOLLOW_POS_FIXED_Z)) pos.z = targetPose.Look().pos.z;

		float3 targetPos = pos + worldOffset;
		ComponentHandle<FollowLeap> leap = a_chunk.GetComponent<FollowLeap>(it);
		if (!leap.IsValid())
		{

			result->newPos = targetPos;
			continue;
		}

		if (!(followData.Look().flag & FOLLOW_POS_FIXED_X)) pos.x = targetPose.Look().pos.x;
		if (!(followData.Look().flag & FOLLOW_POS_FIXED_Y)) pos.y = targetPose.Look().pos.y;
		if (!(followData.Look().flag & FOLLOW_POS_FIXED_Z)) pos.z = targetPose.Look().pos.z;

		ComponentHandle<Pose> thisPose = a_chunk.GetComponent<Pose>(it);
		if (!thisPose.IsValid()) continue;
		ComponentHandle<MotionResult> motionResult = a_chunk.GetComponent<MotionResult>(it);
		if (!motionResult.IsValid()) continue;

		// 対象への移動量を取得
		float3 toTargetDis = targetPos - thisPose.Look().pos;
		// 座標固定だった場合移動量を0にする
		if ((followData.Look().flag & FOLLOW_POS_FIXED_X)) toTargetDis.x = 0.0f;
		if ((followData.Look().flag & FOLLOW_POS_FIXED_Y)) toTargetDis.y = 0.0f;
		if ((followData.Look().flag & FOLLOW_POS_FIXED_Z)) toTargetDis.z = 0.0f;

		float3 normalizeVector = Normalize(toTargetDis);

		

		float3 maxSpeed = leap.Look().followLeap;

		// 移動量を良い感じにする(調整中)
		float3 moveRate;
		if (toTargetDis.x != 0.0f) moveRate.x = std::pow(toTargetDis.x / 1.2f, 2.0f);
		if (toTargetDis.y != 0.0f) moveRate.y = std::pow(toTargetDis.y / 1.2f, 2.0f);
		if (toTargetDis.z != 0.0f) moveRate.z = std::pow(toTargetDis.z / 1.2f, 2.0f);

		float3 move;
		move.x = normalizeVector.x * maxSpeed.x * moveRate.x * a_context.deltaTime;
		move.y = normalizeVector.y * maxSpeed.y * moveRate.y * a_context.deltaTime;
		move.z = normalizeVector.z * maxSpeed.z * moveRate.z * a_context.deltaTime;
		
		if (abs(move.x) > abs(toTargetDis.x)) move.x = toTargetDis.x;
		if (abs(move.y) > abs(toTargetDis.y)) move.y = toTargetDis.y;
		if (abs(move.z) > abs(toTargetDis.z)) move.z = toTargetDis.z;


		motionResult->posOffset += move;
	};


	// 回Entityの回転を追従するEntityを取得
	ComponentView followRotView = a_chunk.GetView<ComponentTypes<FollowRotation, FixedResult>>();
	for (auto it : followRotView)

	{
		// 各コンポーネントを取得
		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(it);
		const ComponentHandle<FollowRotation> followData = a_chunk.GetComponent<FollowRotation>(it);
		float3 offset = followData.Look().rotationOffset;

		// 対象がPoseを持っていなければ抜ける
		ComponentHandle<Pose> targetPose = a_chunk.GetComponent<Pose>(followData.Look().targetEntity);
		if (!targetPose.IsValid()) continue;
		// 固定されていなければ追従
		float3 rot(0.0f, 0.0f, 0.0f);

		if (!(followData.Look().flag & FOLLOW_POS_FIXED_X)) rot.x = targetPose->rot.x;
		if (!(followData.Look().flag & FOLLOW_POS_FIXED_Y)) rot.y = targetPose->rot.y;
		if (!(followData.Look().flag & FOLLOW_POS_FIXED_Z)) rot.z = targetPose->rot.z;

		result->newRot = rot + offset;
	}
}


void TrackingWarpSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<TrackingWarp, MotionResult, Position>>();

	for (auto it : view)
	{
		// 自身のワープ情報と座標を取得
		const ComponentHandle<TrackingWarp> tracking = a_chunk.GetComponent<TrackingWarp>(it);
		const ComponentHandle<Position> thisPosition = a_chunk.GetComponent<Position>(it);
		ComponentHandle<MotionResult> result = a_chunk.GetComponent<MotionResult>(it);

		// Targetの座標を取得
		const ComponentHandle<Position> targetPosition = a_chunk.GetComponent<Position>(tracking.Look().targetEntity);
		if (!targetPosition.IsValid()) continue;

		float3 warpDis = tracking.Look().warpDis;

		float disX = thisPosition.Look().x - targetPosition.Look().x;
		float disY = thisPosition.Look().y - targetPosition.Look().y;
		float disZ = thisPosition.Look().z - targetPosition.Look().z;

		float signX = Sign(warpDis.x);
		float signY = Sign(warpDis.y);
		float signZ = Sign(warpDis.z);

		BitFlag flag = tracking.Look().flag;
		bool isFixedX = flag & TRACKING_FLAG::TRACK_FIXED_X;
		bool isFixedY = flag & TRACKING_FLAG::TRACK_FIXED_Y;
		bool isFixedZ = flag & TRACKING_FLAG::TRACK_FIXED_Z;
		bool isNonTracking = flag & TRACKING_FLAG::TRACK_NONTRACK;

		if (
			disX * signX >= abs(warpDis.x) &&
			disY * signY >= abs(warpDis.y) &&
			disZ * signZ >= abs(warpDis.z)
			)
		{
			// 追跡する場合のみTargetの座標を使う
			float3 trackPosition = isNonTracking ? TOFLOAT3(thisPosition.Look()) : TOFLOAT3(targetPosition.Look());
			result->isWarp = true;
			result->warpPos.x = isFixedX ? thisPosition.Look().x : trackPosition.x + tracking.Look().trackOffset.x;
			result->warpPos.y = isFixedY ? thisPosition.Look().y : trackPosition.y + tracking.Look().trackOffset.y;
			result->warpPos.z = isFixedZ ? thisPosition.Look().z : trackPosition.z + tracking.Look().trackOffset.z;
		}

	}
}

void LeapSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView posView = a_chunk.GetView<ComponentTypes<LeapPosComponent, FixedResult>>();

	for (auto it : posView)
	{

		ComponentHandle<LeapPosComponent> leap = a_chunk.GetComponent<LeapPosComponent>(it);
		if (!leap.Look().isActicv) continue;
		leap->leapTime = std::min(leap.Look().leapTime + a_context.deltaTime, leap.Look().leapMaxTime);
		float rateCmp = leap.Look().leapTime / leap.Look().leapMaxTime;
		leap->leapRate = sin(rateCmp * PI / 2);

		const float3 startPos = leap.Look().leapStartPos;
		const float3 endPos = leap.Look().leapEndPos;
		const float rate = leap.Look().leapRate;
		float3 newPos = (endPos - startPos) * rate + startPos;

		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(it);

		result->newPos.x = newPos.x;
		result->newPos.y = newPos.y;
		result->newPos.z = newPos.z;
		if (leap.Look().leapTime == leap.Look().leapMaxTime)
		{
			leap->isActicv = false;
			a_chunk.DeleteChunkComponent(it, PosePosState::kTypeId);

			ComponentHandle<MotionResult> motionResult = a_chunk.GetComponent<MotionResult>(it);
			ResetFixedPosState(result, motionResult);
			
		}
	}

	ComponentView view = a_chunk.GetView<ComponentTypes<LeapRotComponent, FixedResult>>();

	for (auto it : view)
	{

		ComponentHandle<LeapRotComponent> leap = a_chunk.GetComponent<LeapRotComponent>(it);
		if (!leap.Look().isActicv) continue;
		leap->leapTime = std::min(leap.Look().leapTime + a_context.deltaTime, leap.Look().leapMaxTime);
		float rateCmp = leap.Look().leapTime / leap.Look().leapMaxTime;
		leap->leapRate = sin(rateCmp * PI / 2);

		const float3 startRot = leap.Look().leapStartRot;
		const float3 endRot = leap.Look().leapEndRot;
		const float rate = leap.Look().leapRate;
		float3 newRot = (endRot - startRot) * rate + startRot;

		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(it);

		result->newRot.x = newRot.x;
		result->newRot.y = newRot.y;
		result->newRot.z = newRot.z;
		if (leap.Look().leapTime == leap.Look().leapMaxTime)
		{
			leap->isActicv = false;
		}
	}
}

void ShakingSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<ShakingComponent>>();

	for (auto it : view)
	{
		ComponentHandle<ShakingComponent> shaking = a_chunk.GetComponent<ShakingComponent>(it);


		float shakingPos = shaking.Look().heightOffset;
		if (shaking.Look().isWait)
		{
			shaking->shankingDegree = (PI / 2.0f) * 3.0;
			shakingPos += std::sin(shaking->shankingDegree) * shaking.Look().shakingDisplacement + shaking->heightOffset;
		}
		else
		{
			shaking->shankingDegree = std::remainder(shaking.Look().shankingDegree + a_context.deltaTime * shaking.Look().shakingSpeed, 2 * PI);
			shakingPos += std::sin(shaking->shankingDegree) * shaking.Look().shakingDisplacement + shaking->heightOffset;
		}

		switch (shaking.Look().shakingType)
		{
		case SHAKING_POSITION:
		{
			ComponentHandle<EphemeralResult> result = a_chunk.GetComponent<EphemeralResult>(it);
			if (!result.IsValid()) continue;
			result->posOffset.y = shakingPos;
			break;
		}


		case SHAKING_MODEL:
		{
			ComponentHandle<ModelKey> modelKey = a_chunk.GetComponent<ModelKey>(it);
			if (!modelKey.IsValid()) continue;
			modelKey->offset.y = shakingPos;
			break;
		}

		}

	}
}

void LookSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<LookComponent, Position, FixedResult>>();

	for (auto it : view)
	{
		const ComponentHandle<LookComponent> look = a_chunk.GetComponent<LookComponent>(it);
		ComponentHandle<FixedResult> lookResult = a_chunk.GetComponent<FixedResult>(it);


		float3 finalRot = GetLookRot(a_chunk, it, look.Look().target);

		lookResult->newRot.x = finalRot.x;
		lookResult->newRot.y = finalRot.y;
		lookResult->newRot.z = 0.0f;
	}

}

void ShakeSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<ShakeComponent, EphemeralResult, Pose>>();

	for (auto it : view)
	{
		ComponentHandle<ShakeComponent> shake = a_chunk.GetComponent<ShakeComponent>(it);
		shake->shakeTime -= a_context.effectStepTime;
		shake->shakeTime = std::max(shake->shakeTime, 0.0f);

		ComponentHandle<EphemeralResult> result = a_chunk.GetComponent<EphemeralResult>(it);
		if (shake.Look().shakeTime <= 0.0f)
		{
			continue;
		}

		shake->elapsedTime += a_context.effectStepTime;

		// 移動を求めるために行列を作成
		DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();

		// 角度を持っている場合は向いてる方向に進むようにする
		ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(it);

		float powerRate = 1.0f;
		powerRate = Lerp(1.0f, 0.5f, shake.Look().elapsedTime / (shake.Look().elapsedTime + shake.Look().shakeTime));


		matrix *= DirectX::XMMatrixTranslation(
			Noise((shake.Look().elapsedTime + kNoiseXTimeOffsert) * shake.Look().shakeAmplitude.x) * shake.Look().shakePower.x * powerRate,
			Noise((shake.Look().elapsedTime + kNoiseYTimeOffsert) * shake.Look().shakeAmplitude.y) * shake.Look().shakePower.y * powerRate,
			Noise((shake.Look().elapsedTime + kNoiseZTimeOffsert) * shake.Look().shakeAmplitude.z) * shake.Look().shakePower.z * powerRate
		);

		// 回転をかける
		matrix *= DirectX::XMMatrixRotationRollPitchYaw(rot.Look().pitch, rot.Look().yaw, rot.Look().roll);

		DirectX::XMFLOAT3 offset;
		DirectX::XMVECTOR vector = matrix.r[3];
		DirectX::XMStoreFloat3(&offset, vector);

		result->posOffset.x += offset.x;
		result->posOffset.y += offset.y;
		result->posOffset.z += offset.z;
	}
}

void PhysicsPoseSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<Velocity, Pose, FixedResult, MotionResult, EphemeralResult>>();

	for (auto it : view)
	{
		SetPosePos(it, a_chunk);

		ComponentHandle<MotionResult> motionResult = a_chunk.GetComponent<MotionResult>(it);
		motionResult->posOffset = float3(0.0f, 0.0f, 0.0f);
		motionResult->rotOffset = float3(0.0f, 0.0f, 0.0f);
		motionResult->isWarp = false;
		motionResult->warpPos = float3(0.0f, 0.0f, 0.0f);

		ComponentHandle<EphemeralResult> ephemeralResult = a_chunk.GetComponent<EphemeralResult>(it);
		ephemeralResult->posOffset = float3(0.0f, 0.0f, 0.0f);
		ephemeralResult->rotOffset = float3(0.0f, 0.0f, 0.0f);
	}
}

void LatePhysicsPoseSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// !!!New!!!
	// ColliderBack の MotionResult を Pose に反映してから Position へ確定する。
	ComponentView view = a_chunk.GetView<ComponentTypes<Velocity, Pose, FixedResult, MotionResult, EphemeralResult, Position>>();

	for (auto it : view)
	{
		SetPosePos(it, a_chunk);

		ComponentHandle<MotionResult> motionResult = a_chunk.GetComponent<MotionResult>(it);
		motionResult->posOffset = float3(0.0f, 0.0f, 0.0f);
		motionResult->isWarp = false;
		motionResult->warpPos = float3(0.0f, 0.0f, 0.0f);

		const ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(it);
		ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);

		pos->x = pose.Look().pos.x;
		pos->y = pose.Look().pos.y;
		pos->z = pose.Look().pos.z;
	}
}

void PoseSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// Viewを使って表示に必要な座標と角度を更新
	ComponentView posView = a_chunk.GetView<ComponentTypes<Pose, FixedResult, MotionResult, EphemeralResult>>();
	// 先に処理
	std::vector<PosePosStateEnum>setPosStatus = {
	POSE_POS_RAIL,
	POSE_POS_LEAP,
	POSE_POS_NONE
	};

	// 遅延前座標を決定
	for (auto it : posView)
	{
		ComponentHandle<PosePosState> posState = a_chunk.GetComponent<PosePosState>(it);
				// 先に処理する必要がある場合はIPoseを使う
		if ((posState.IsValid() && IsSetFixedPos(posState, setPosStatus)) || !posState.IsValid())
		{
			SetPosePos(it, a_chunk);
		}
		// 遅延処理の場合は現在のPositionを代入
		else
		{
			SetCurrentPosePos(it, a_chunk);
		}
	}

	// 角度のViewを取得
	ComponentView rotView = a_chunk.GetView<ComponentTypes<Pose, FixedResult, MotionResult, EphemeralResult>>();
	// 先に処理
	std::vector<PoseRotStateEnum>setRotStatus = {
	POSE_ROT_NONE
	};

	// 遅延前角度を決定
	for (auto it : rotView)
	{
		ComponentHandle<PoseRotState> rotState = a_chunk.GetComponent<PoseRotState>(it);
		// 先に処理する必要がある場合はIPoseを使う
		if ((rotState.IsValid() && IsSetFixedRot(rotState, setRotStatus)) || !rotState.IsValid())
		{
			SetPoseRot(it, a_chunk);
		}
		// 遅延処理の場合は現在のRotationを代入
		else 
		{
			SetCurrentPoseRot(it, a_chunk);
		}
	}
}

void LatePoseSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// Viewを使って表示に必要な座標と角度を更新
	ComponentView posView = a_chunk.GetView<ComponentTypes<Pose, FixedResult, MotionResult, EphemeralResult>>();
	// 遅延
	std::vector<PosePosStateEnum>setPosStatus = {
		POSE_POS_CAMERA,
		POSE_POS_DEBUGCAMERA,
		POSE_POS_FOLLOW
	};

	// 遅延座標を決定
	for (auto it : posView)
	{
		ComponentHandle<PosePosState> posState = a_chunk.GetComponent<PosePosState>(it);

		if (posState.IsValid() && IsSetFixedPos(posState, setPosStatus))
		{
			SetPosePos(it, a_chunk);
		}
	}

	// 角度のViewを取得
	ComponentView rotView = a_chunk.GetView<ComponentTypes<Pose, FixedResult, MotionResult, EphemeralResult>>();
	// 遅延
	std::vector<PoseRotStateEnum>setRotStatus = {
		POSE_ROT_CAMERA,
		POSE_ROT_DEBUGCAMERA,
		POSE_ROT_FOLLOW,
		POSE_ROT_LOOK,
		POSE_ROT_LOOKMOVE
	};

	// 遅延前角度を決定
	for (auto it : rotView)
	{
		ComponentHandle<PoseRotState> rotState = a_chunk.GetComponent<PoseRotState>(it);

		if (rotState.IsValid() && IsSetFixedRot(rotState, setRotStatus))
		{
			SetPoseRot(it, a_chunk);
		}
	}
}


void TransformSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// PoseをTransformに反映
	ApplyPoseToTransform(a_chunk);
}


void SetPosePos(Entity a_entity, Chunk& a_chunk)
{
	// Poseを取得
	ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(a_entity);

		// 各Resultを取得
	ComponentHandle<FixedResult> fixedResult = a_chunk.GetComponent<FixedResult>(a_entity);
	ComponentHandle<MotionResult> motionResult = a_chunk.GetComponent<MotionResult>(a_entity);
	ComponentHandle<MotionTransform> motionTransform = a_chunk.GetComponent<MotionTransform>(a_entity);
	ComponentHandle<EphemeralResult> ephemeralResult = a_chunk.GetComponent<EphemeralResult>(a_entity);

		// Stateを取得
	ComponentHandle<PosePosState> posePosState = a_chunk.GetComponent<PosePosState>(a_entity);

		// 基本座標を
	pose->pos = fixedResult.Look().newPos;
		// 自動移動分を更新して加算
	if (motionResult.Look().isWarp)
	{
		motionTransform->motionPos = motionResult.Look().warpPos - pose->pos;
	}
	motionTransform->motionPos += motionResult.Look().posOffset;
	pose->pos += motionTransform.Look().motionPos;

	// 1フレーム分の揺れを加算
	pose->pos += ephemeralResult.Look().posOffset;

}

void SetPoseRot(Entity a_entity, Chunk& a_chunk)
{		
	// Poseを取得
	ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(a_entity);

		// 各Resultを取得
	ComponentHandle<FixedResult> fixedResult = a_chunk.GetComponent<FixedResult>(a_entity);
	ComponentHandle<MotionResult> motionResult = a_chunk.GetComponent<MotionResult>(a_entity);
	ComponentHandle<MotionTransform> motionTransform = a_chunk.GetComponent<MotionTransform>(a_entity);
	ComponentHandle<EphemeralResult> ephemeralResult = a_chunk.GetComponent<EphemeralResult>(a_entity);

		// Stateを取得
	ComponentHandle<PoseRotState> poseRotState = a_chunk.GetComponent<PoseRotState>(a_entity);

		// 基本座標を
	pose->rot = fixedResult.Look().newRot;
		// 自動回転分を加算
	motionTransform->motionRot += motionResult.Look().rotOffset;
	ComponentHandle<AngleLimitComponent> angleLimit = a_chunk.GetComponent<AngleLimitComponent>(a_entity);
	if (angleLimit.IsValid())
	{
		if (angleLimit.Look().isLimitX) motionTransform->motionRot.y = std::clamp(motionTransform->motionRot.y, angleLimit.Look().minAngle.x, angleLimit.Look().maxAngle.x);
		if (angleLimit.Look().isLimitY) motionTransform->motionRot.x = std::clamp(motionTransform->motionRot.x, angleLimit.Look().minAngle.y, angleLimit.Look().maxAngle.y);
	}

	pose->rot += motionTransform.Look().motionRot;
		// 1フレーム分の揺れを加算
	pose->rot += ephemeralResult.Look().rotOffset;
	
}


void SetCurrentPosePos(Entity a_entity, Chunk& a_chunk)
{
	ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(a_entity);
	ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(a_entity);
	if (!pose.IsValid()) return;

	pose->pos = TOFLOAT3(pos.Look());
}

void SetCurrentPoseRot(Entity a_entity, Chunk& a_chunk)
{
	ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(a_entity);
	ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(a_entity);

	pose->rot = TOFLOAT3(rot.Look());
}

bool IsSetFixedPos(const ComponentHandle<PosePosState>& a_posePosState, const std::vector<PosePosStateEnum>& a_setPoseState)
{
	for (auto stateIt : a_setPoseState)
	{
		if (a_posePosState.Look().state != stateIt)
		{
			continue;
		}

		return true;
	}

	return false;
}

bool IsSetFixedRot(const ComponentHandle<PoseRotState>& a_poseRotState, const std::vector<PoseRotStateEnum>& a_setPoseState)
{
	for (auto stateIt : a_setPoseState)
	{
		if (a_poseRotState.Look().state != stateIt)
		{
			continue;
		}

		return true;
	}

	return false;
}

float3 GetLookRot(Chunk& a_chunk, const Entity user, const Entity target)
{
	const float3 position = GetEntityPosePos(a_chunk, user);

	const float3 targetPos = GetEntityPosePos(a_chunk, target);
	
	
	float3 relativeDistance(
		targetPos.x - position.x,
		targetPos.y - position.y,
		targetPos.z - position.z
	);

	float targetDistance = NormalizeLength(
		relativeDistance.x,
		relativeDistance.y,
		relativeDistance.z
	);

	if (targetDistance == std::numeric_limits<float>::max())
	{
		return float3();
	}

	float pitchSign = Sign(relativeDistance.y) * -1.0f;

	float dirXZ =
		std::sqrtf(
			std::pow(relativeDistance.x, 2.0f) + std::pow(relativeDistance.z, 2.0f)
		);

	float dirY = std::sqrtf(
		std::pow(relativeDistance.y, 2.0f)
	);

	float finalPitch = atan2f(dirY, dirXZ) * (180 / PI) * pitchSign;
	float finalYaw = atan2f(relativeDistance.x, relativeDistance.z) * (180 / PI);

	return float3(finalPitch, finalYaw, 0.0f);
}


//void CurrentPosePos(const Entity& a_entity, Chunk& a_chunk)
//{
//	ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(a_entity);
//
//	ComponentHandle<Position> currentPosition = a_chunk.GetComponent<Position>(a_entity);
//	if (currentPosition.IsValid()) pose->pos = TOFLOAT3(currentPosition.Look());
//}
//
//void CurrentPoseRot(const Entity& a_entity, Chunk& a_chunk)
//{
//	ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(a_entity);
//
//	ComponentHandle<Rotation> currentRotation = a_chunk.GetComponent<Rotation>(a_entity);
//	if (currentRotation.IsValid()) pose->rot = TOFLOAT3(currentRotation.Look());
//
//}

void ApplyPoseToTransform(Chunk& a_chunk)
{
	ComponentView poseView = a_chunk.GetView<ComponentTypes<Pose>>();
	for (auto it : poseView)
	{
		ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(it);
		ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
		if (pos.IsValid())
		{
			pos->x = pose.Look().pos.x;
			pos->y = pose.Look().pos.y;
			pos->z = pose.Look().pos.z;
		}
		ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(it);
		if (rot.IsValid())
		{
			rot->pitch = pose.Look().rot.x;
			rot->yaw = pose.Look().rot.y;
			rot->roll = pose.Look().rot.z;
		}
	}
}

void MoveForwardSystem(Chunk& a_chunk, const SystemContext& a_context)
{

	ComponentView view = a_chunk.GetView<ComponentTypes<MoveForward, MotionResult, Rotation>>();

	for (auto it : view)
	{
		ComponentHandle<MotionResult> result = a_chunk.GetComponent<MotionResult>(it);
		const ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(it);
		ComponentHandle<MoveForward> move = a_chunk.GetComponent<MoveForward>(it);

		move->speed *= pow(move.Look().attenuation, a_context.deltaTime);

		float3 radRotation(rot.Look().pitch * RAD, rot.Look().yaw * RAD, rot.Look().roll * RAD);

		float3 forward;
		forward.x = std::sin(radRotation.y) * std::cos(radRotation.x);
		forward.y = std::sin(-radRotation.x);
		forward.z = std::cos(radRotation.y) * std::cos(radRotation.x);

		float3 displacement = forward * move.Look().speed * a_context.deltaTime;

		result->posOffset.x += displacement.x;
		result->posOffset.y += displacement.y;
		result->posOffset.z += displacement.z;
	}
}
