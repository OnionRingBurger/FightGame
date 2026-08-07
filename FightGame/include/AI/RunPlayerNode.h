#pragma once

#include "ActionNode.h"

// !!!New!!!
class RunPlayerNode : public ActionNode
{
public:
	RunPlayerNode();
	~RunPlayerNode() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;
};
