#pragma once

#include "ActionNode.h"

// !!!New!!!
// ノード固定の duration 経過まで RUNNING、満了で SUCCESS
// BB の waitTime 経過まで RUNNING、満了で SUCCESS !!!Old!!!
class WaitAction : public ActionNode
{
public:
	explicit WaitAction(float a_maxDuration);
	~WaitAction() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;

private:
	// 継続時間
	float maxDuration;
};
