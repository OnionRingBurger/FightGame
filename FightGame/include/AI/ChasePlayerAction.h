#pragma once

#include "ActionNode.h"

// プレイヤー方向へ移動し、射程内かつある程度正面なら攻撃する
class ChasePlayerAction : public ActionNode
{
public:
	ChasePlayerAction(float a_magnitube);
	~ChasePlayerAction() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;

private :
	float magnitube;

};