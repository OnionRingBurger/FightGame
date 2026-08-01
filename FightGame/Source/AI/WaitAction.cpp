#include "AI/WaitAction.h"

// !!!New!!!
WaitAction::WaitAction()
{
}

WaitAction::~WaitAction()
{
}

Node::Status WaitAction::Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result)
{
	// 待機中なので行動しない
	result.MoveDir = float2(0.0f, 0.0f);
	result.IsMove = false;
	result.UseAttack = false;

	// 対象がいない場合失敗
	const auto enemyIt = blackBord.enemyDatas.find(context.agentEntity);
	if (enemyIt == blackBord.enemyDatas.end())
	{
		return NODE_FAILURE;
	}

	// 継続時間を更新
	const float waitTime = enemyIt->second.attackData.waitTime;
	context.actionDuration += systemInfo.tickDelta;

	// 終了したら成功
	if (context.actionDuration >= waitTime)
	{
		context.actionDuration = 0.0f;
		return NODE_SUCCES;
	}

	// 何もなかった場合実行中を返す
	return NODE_RUNNING;
}
