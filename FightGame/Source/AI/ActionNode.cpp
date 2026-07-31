#include "AI/ActionNode.h"

// !!!New!!!
ActionNode::ActionNode()
{
}

ActionNode::~ActionNode()
{
}

Node::Status ActionNode::Tick(AIContext& context, const AIBlackboard& blackBord, AIResult& result)
{
	// !!!New!!!
	(void)context;
	(void)blackBord;
	(void)result;
	return NODE_FAILURE;
}