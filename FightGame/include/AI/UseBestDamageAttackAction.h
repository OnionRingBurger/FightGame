#pragma once

#include "ActionNode.h"

// !!!New!!!
class UseBestDamageAttackAction : public ActionNode
{
public:
	UseBestDamageAttackAction();
	~UseBestDamageAttackAction() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;
};
