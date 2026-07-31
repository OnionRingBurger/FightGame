#pragma once

#include "LeafNode.h"

// !!!New!!!
// 葉: 動作を実行するアクションノード
class ActionNode : public LeafNode
{
public:
	ActionNode();
	~ActionNode() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIResult& result) override;
};