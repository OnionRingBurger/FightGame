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
	

	ComponentView cameraView = a_chunk.GetView<ComponentTypes<Camera, Position, Rotation, FixedResult>>();
	for (auto cameraIt : cameraView)
	{
		ComponentHandle<Position> cameraPos = a_chunk.GetComponent<Position>(cameraIt);
		ComponentHandle<Rotation> cameraRot = a_chunk.GetComponent<Rotation>(cameraIt);


		float3 cameraPosF = TOFLOAT3(cameraPos.Look());
		float3 vector = pointPos - cameraPosF;
		float3 normalizeVector = Normalize(vector);

		float moveDis = point.Look().posLeapSpeed * a_context.deltaTime;
		float dis = NormalizeLength(vector.x, vector.y, vector.z);
		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(cameraIt);

		float3 offset =  GetLocalOffset(point->posOffset, pointRot);

		result->newPos = pointPos + offset;

		result->newRot = pointRot;
		

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



