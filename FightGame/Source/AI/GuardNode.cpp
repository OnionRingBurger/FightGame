#include "AI/GuardNode.h"

// !!!New!!!
GuardNode::GuardNode()
{
}

GuardNode::~GuardNode()
{
}

Node::Status GuardNode::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	(void)context;
	(void)mind;
	(void)systemInfo;

	result.MoveDir = float2(0.0f, 0.0f);
	result.IsMove = false;
	result.UseAttack = false;
	result.AttackIndex = 0;
	result.UseGuard = false;

	if (blackBord.isPlayerDead)
	{
		return NODE_FAILURE;
	}

	const auto enemyIt = blackBord.enemyDatas.find(context.agentEntity);
	if (enemyIt == blackBord.enemyDatas.end())
	{
		return NODE_FAILURE;
	}

	result.UseGuard = true;
	return NODE_SUCCES;
}
