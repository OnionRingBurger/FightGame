#include "RailSystem.h"
#include "Components.h"

using namespace Component;

void RailSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView posView = a_chunk.GetView<ComponentTypes<RailUser, FixedResult>>();

	for (auto it : posView)
	{
		ComponentHandle<RailUser> user = a_chunk.GetComponent<RailUser>(it);
		Entity railEntity = user.Look().targetRail;
		ComponentHandle<RailComponent> rail = a_chunk.GetComponent<RailComponent>(railEntity);
		if (!rail.IsValid()) continue;
		ComponentHandle<FixedResult> result = a_chunk.GetComponent<FixedResult>(it);


		result->newPos.x = rail.Look().currentPos.x;
		result->newPos.y = rail.Look().currentPos.y;
		result->newPos.z = rail.Look().currentPos.z;
	}
}

void RailUpdateSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView posView = a_chunk.GetView<ComponentTypes<RailComponent>>();

	for (auto it : posView)
	{

		ComponentHandle<RailComponent> rail = a_chunk.GetComponent<RailComponent>(it);
		if (!rail.Look().isActicv) continue;
		rail->railTime = std::min(rail.Look().railTime + a_context.deltaTime, rail.Look().railMaxTime);
		float rateCmp = rail.Look().railTime / rail.Look().railMaxTime;
		// 補正をかける場合ここにかける、今はいったん消してる
		rail->railRate = rateCmp;

		const float3 startPos = rail.Look().railStartPos;
		const float3 endPos = rail.Look().railEndPos;
		const float rate = rail.Look().railRate;
		float3 newPos = (endPos - startPos) * rate + startPos;


		rail->currentPos = newPos;
		if (rail.Look().railTime == rail.Look().railMaxTime)
		{
			rail->isActicv = false;
		}
	}
}

void FlySystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<RailFly, EphemeralResult>>();

	for (auto it : view)
	{
		ComponentHandle<RailFly> fly = a_chunk.GetComponent<RailFly>(it);
		// 飛行による上下移動分のOffset
		float3 flyOffset(0.0f, 0.0f, 0.0f);
		// サインカーブを使い上下移動を行う
		fly->flyProgress += a_context.deltaTime * fly.Look().flySpeed;
		flyOffset.y = std::sinf(fly.Look().flyProgress) * fly.Look().flyPow;

		// Resultに代入
		ComponentHandle<EphemeralResult> railFlyResult = a_chunk.GetComponent<EphemeralResult>(it);
		railFlyResult->posOffset = flyOffset;
	}
}
