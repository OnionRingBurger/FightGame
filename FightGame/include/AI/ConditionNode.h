#pragma once

#include "LeafNode.h"

// !!!New!!!
// —t: ğŒ”»’èƒm[ƒh
class ConditionNode : public LeafNode
{
public:
	ConditionNode();
	~ConditionNode() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIResult& result) override;
};