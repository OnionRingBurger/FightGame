#include "AI/ActionNode.h"

// !!!New!!!
ActionNode::ActionNode()
{
}

ActionNode::~ActionNode()
{
}

Node::Status ActionNode::Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result)
{
	// !!!New!!!
	(void)context;
	(void)blackBord;
	(void)systemInfo;
	(void)result;
	return NODE_FAILURE;
}
