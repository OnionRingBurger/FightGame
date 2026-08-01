#include "AI/BehaviorTree.h"

// !!!New!!!
BehaviorTree::BehaviorTree()
	: rootNode(std::make_unique<RootNode>())
{
}

BehaviorTree::~BehaviorTree()
{
}

void BehaviorTree::Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result)
{
	// !!!New!!!
	if (!rootNode)
	{
		return;
	}
	if (rootNode->Tick(context, blackBord, systemInfo, result) != Node::NODE_RUNNING)
	{
		context.actionDuration = 0.0f;
		context.nodeIndexes.clear();
	}
}

RootNode& BehaviorTree::GetRoot()
{
	return *rootNode;
}