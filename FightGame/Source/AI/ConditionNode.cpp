#include "AI/ConditionNode.h"

// !!!New!!!
ConditionNode::ConditionNode()
{
}

ConditionNode::~ConditionNode()
{
}

Node::Status ConditionNode::Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result)
{
	// !!!New!!!
	(void)context;
	(void)blackBord;
	(void)systemInfo;
	(void)result;
	return NODE_FAILURE;
}
