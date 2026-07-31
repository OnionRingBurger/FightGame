#include "AI/ConditionNode.h"

// !!!New!!!
ConditionNode::ConditionNode()
{
}

ConditionNode::~ConditionNode()
{
}

Node::Status ConditionNode::Tick(AIContext& context, const AIBlackboard& blackBord, AIResult& result)
{
	// !!!New!!!
	(void)context;
	(void)blackBord;
	(void)result;
	return NODE_FAILURE;
}