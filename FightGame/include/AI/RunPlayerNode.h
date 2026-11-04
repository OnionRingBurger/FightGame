#pragma once

#include "ActionNode.h"

// !!!New!!!
class RunPlayerNode : public ActionNode
{
public:
	RunPlayerNode(float a_magnitube);
	~RunPlayerNode() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result, float deltaTime) override;

private:
	float magnitube;
};
