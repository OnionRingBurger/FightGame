#pragma once

#include "ActionNode.h"

// !!!New!!!
// プレイヤー方向へ移動し、射程内かつある程度正面なら攻撃する
class ChasePlayerAction : public ActionNode
{
public:
	ChasePlayerAction();
	~ChasePlayerAction() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIResult& result) override;
};