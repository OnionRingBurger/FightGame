#pragma once

#include "ActionNode.h"

// !!!New!!!
class GuardNode : public ActionNode
{
public:
	GuardNode();
	~GuardNode() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;
};
