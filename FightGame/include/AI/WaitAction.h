#pragma once

#include "ActionNode.h"

// !!!New!!!
// BB ÇÃ waitTime åoâﬂÇ‹Ç≈ RUNNINGÅAñûóπÇ≈ SUCCESS
class WaitAction : public ActionNode
{
public:
	WaitAction();
	~WaitAction() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result) override;
};
