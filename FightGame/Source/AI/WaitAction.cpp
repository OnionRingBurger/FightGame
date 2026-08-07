#include "AI/WaitAction.h"

// !!!New!!!
WaitAction::WaitAction(float a_maxDuration)
	: maxDuration(a_maxDuration)
{
}

WaitAction::~WaitAction()
{
}

Node::Status WaitAction::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	(void)blackBord;
	(void)mind;

	// 待機中なので行動しない
	result.MoveDir = float2(0.0f, 0.0f);
	result.IsMove = false;
	result.UseAttack = false;

	// !!!New!!!
	context.actionDuration += systemInfo.tickDelta;

	// 終了したら成功
	if (context.actionDuration >= maxDuration)
	{
		context.actionDuration = 0.0f;
		return NODE_SUCCES;
	}

	// 何もなかった場合実行中を返す
	return NODE_RUNNING;
}
