#include "AI/ConditionNode.h"

// !!!New!!!
ConditionNode::ConditionNode()
{
}

ConditionNode::~ConditionNode()
{
}

Node::Status ConditionNode::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	return NODE_FAILURE;
}

