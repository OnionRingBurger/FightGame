#include "DebugCameraSystem.h"
#include "Components.h"

using namespace Component;

void DebugCameraSystem(Chunk& a_chunk)
{
	ComponentView pointView = a_chunk.GetView<ComponentTypes<CameraPoint>>();
	ComponentHandle<CameraPoint> point;
	Entity targetEntity = kInvalidEntity;

	for (auto pointIt : pointView)
	{
		ComponentHandle<CameraPoint> currentPoint = a_chunk.GetComponent<CameraPoint>(pointIt);
		if (!point.IsValid() ||
			currentPoint.Look().cameraPointPriority > point.Look().cameraPointPriority)
		{
			point = a_chunk.GetComponent<CameraPoint>(pointIt);
			targetEntity = pointIt;
		}
	}

	if (targetEntity == kInvalidEntity)
	{
		return;
	}

	ComponentView view = a_chunk.GetView<ComponentTypes<DebugCameraTag>>();
	for (auto it : view)
	{
		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(it);
		
		ComponentHandle<Pose> targetPose = a_chunk.GetComponent<Pose>(targetEntity);

		result->newPos = targetPose.Look().pos;
		result->newRot = targetPose.Look().rot;
		
	}
}

void DebugInputSystem(Chunk& a_chunk)
{
}


