#include "AI/ConditionNode.h"

// !!!New!!!
ConditionNode::ConditionNode()
{
}

ConditionNode::~ConditionNode()
{
}

Node::Status ConditionNode::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result, float deltaTime)
{
	(void)context;
	(void)blackBord;
	(void)mind;
	(void)systemInfo;
	(void)result;
	(void)deltaTime;
	return NODE_FAILURE;
}

