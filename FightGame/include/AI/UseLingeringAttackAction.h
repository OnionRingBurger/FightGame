#pragma once

#include "ActionNode.h"

// !!!New!!!
class UseLingeringAttackAction : public ActionNode
{
public:
	UseLingeringAttackAction();
	~UseLingeringAttackAction() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;
};
