#pragma once

#include "ActionNode.h"

// !!!New!!!
class UseAttackAction : public ActionNode
{
public:
	UseAttackAction();
	~UseAttackAction() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;
};
