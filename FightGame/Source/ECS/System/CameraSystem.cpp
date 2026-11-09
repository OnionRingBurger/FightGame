#include "CameraSystem.h"
#include "Components.h"
#include "MathAssist.h"
#include "SystemAssist.h"


using namespace Component;

bool SafeGetTargetTransform(const Entity& a_entity, Chunk& a_chunk, float3& a_pos, float3& a_rot)
{
	ComponentHandle<Pose> pose = a_chunk.GetComponent<Pose>(a_entity);
	if (pose.IsValid())
	{
		a_pos = pose.Look().pos;
		a_rot = pose.Look().rot;
		return true;
	}

	ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(a_entity);
	ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(a_entity);
	if (pos.IsValid())
	{
		a_pos = TOFLOAT3(pos.Look());
		a_rot = TOFLOAT3(rot.Look());
		return true;
	}

	return false;
}

void CameraViewSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<Position, Camera>>();

	for (auto it : view)
	{
		const ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
		ComponentHandle<Camera> camera = a_chunk.GetComponent<Camera>(it);

		if (!pos.IsValid() || !camera.IsValid()) continue;

		const ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(it);


		DirectX::XMMATRIX rot = DirectX::XMMatrixIdentity();

		if (rotation.IsValid())
		{
			// 行列を使って注視点を求める
			float pitch = rotation.Look().pitch * RAD;
			float yaw = rotation.Look().yaw * RAD;
			float roll = rotation.Look().roll * RAD;

			rot = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

		}


		DirectX::XMVECTOR forwardVector = DirectX::XMVector3TransformNormal({ 0,0,1 }, rot);
		DirectX::XMVECTOR localViewVector = DirectX::XMVectorScale(forwardVector, 300.0f);



		DirectX::XMFLOAT3 localView;
		DirectX::XMStoreFloat3(&localView, localViewVector);


		camera->lookPosition = float3{ pos.Look().x + localView.x, pos.Look().y + localView.y, pos.Look().z + localView.z };

	}
}

void UpdateTargetCameraPoint(Chunk& a_chunk, Entity a_camera, Entity a_point)
{
	ComponentHandle<CameraMover> mover = a_chunk.GetComponent<CameraMover>(a_camera);
	if (mover.Look().targetPoint == a_point) return;
	
	mover->targetPoint = a_point;
	mover->leapProgress = 0.0f;
	mover->leapEnd = false;


	ComponentHandle<Pose> pointPose = a_chunk.GetComponent<Pose>(a_point);

	float sign = Sign(pointPose.Look().pos.x - mover.Look().finalTargetPos.x);
	if (sign == 0.0f) sign = 1.0f;
	float2 startPos(3.0f * sign, 0.0f);
	float2 endPos(-7.0f * sign, 0.0f);


	ComponentHandle<CameraChangeEffect> effect = a_chunk.GetComponent<CameraChangeEffect>(a_point);
	if (!effect.IsValid()) return;
	a_chunk.CreateNewEntity(
		UIComponent(effect.Look().effectKey, startPos, float2(3.5f, 3.0f), 0.0f),
		UIPosLerp(startPos, endPos, 60.0f),
		LifeTime(70.0f)
	);

	
}

void CameraChase(Chunk& a_chunk, const SystemContext& a_context, Entity a_camera, Entity a_point, float3& retPos, float3& retRot)
{
	// Poseを取得
	ComponentHandle<Pose> pointPose = a_chunk.GetComponent<Pose>(a_point);
	ComponentHandle<CameraMover> mover = a_chunk.GetComponent<CameraMover>(a_camera);

	// もし時間が最大だった場合直接追従する
	if (mover.Look().leapEnd)
	{
		retPos = pointPose.Look().pos;
		retRot = pointPose.Look().rot;
		mover->finalTargetPos = pointPose.Look().pos;
		mover->finalTargetRot = pointPose.Look().rot;
		return;
	}

	// CameraPointの指定時間でLeapする
	ComponentHandle<Pose> cameraPose = a_chunk.GetComponent<Pose>(a_camera);
	mover->leapProgress += a_context.deltaTime / 15.0f;
	mover->leapProgress = std::clamp(mover.Look().leapProgress, 0.01f, 1.0f);
	if (mover.Look().leapProgress >= 1.0f)
	{
		retPos = pointPose.Look().pos;
		retRot = pointPose.Look().rot;
		mover->finalTargetPos = pointPose.Look().pos;
		mover->finalTargetRot = pointPose.Look().rot;
		mover->leapEnd = true;
		return;
	}

	
	retPos = Lerp(mover.Look().finalTargetPos, pointPose.Look().pos, mover.Look().leapProgress);

	retRot = Lerp(mover.Look().finalTargetRot, pointPose.Look().rot, mover.Look().leapProgress);

}

void CameraMoveSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView pointView = a_chunk.GetView<ComponentTypes<CameraPoint>>();

	ComponentHandle<CameraPoint> point;
	Entity pointEntity = kInvalidEntity;
	for (auto pointIt : pointView)
	{
		ComponentHandle<CameraPoint> currentPoint = a_chunk.GetComponent<CameraPoint>(pointIt);
		if (!point.IsValid() ||
			currentPoint.Look().cameraPointPriority > point.Look().cameraPointPriority)
		{
			point = a_chunk.GetComponent<CameraPoint>(pointIt);
			pointEntity = pointIt;
		}
	}

	// 値を取得できなかった場合return;
	float3 pointPos;
	float3 pointRot;

	bool isTargetSafe = SafeGetTargetTransform(pointEntity, a_chunk, pointPos, pointRot);
	// Pointの値が取得できなかったため抜ける
	if (!isTargetSafe)
	{
		return;
	}
	

	ComponentView cameraView = a_chunk.GetView<ComponentTypes<Camera, CameraMover, Position, Rotation, FixedResult>>();
	for (auto cameraIt : cameraView)
	{
		UpdateTargetCameraPoint(a_chunk, cameraIt, pointEntity);
		float3 newPos;
		float3 newRot;
		CameraChase(a_chunk, a_context, cameraIt, pointEntity, newPos, newRot);
		
		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(cameraIt);

		// ローカルのオフセットを取得
		float3 offset =  GetLocalOffset(point->posOffset, pointRot);

		// 結果を代入
		result->newPos = newPos + offset;
		result->newRot = newRot;
		

	}
}

void ZoomMoveSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<ZoomComponent, ZoomMove>>();

	for (auto it : view)
	{
		ComponentHandle<ZoomComponent> zoom = a_chunk.GetComponent<ZoomComponent>(it);
		ComponentHandle<ZoomMove> move = a_chunk.GetComponent<ZoomMove>(it);
		// zoom速度を加速
		move->zoomSpeed += move.Look().zoomF * a_context.effectStepTime;

		// zoom量を加算
		zoom->zoom += move.Look().zoomSpeed * a_context.effectStepTime;
		// タイプに合わせ他処理を行う

		// タイプに合わせ他処理を行う
		if (move.Look().moveType == ZOOMMOVE_ZERORESET && zoom.Look().zoom <= zoom.Look().minZoom)
		{
			move->zoomSpeed = 0.0f;
			move->zoomF = 0.0f;
		}
	}
}



