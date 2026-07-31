#include "AI/BehaviorTree.h"

// !!!New!!!
BehaviorTree::BehaviorTree()
	: rootNode(std::make_unique<RootNode>())
{
}

BehaviorTree::~BehaviorTree()
{
}

void BehaviorTree::Tick(AIContext& context, const AIBlackboard& blackBord, AIResult& result)
{
	// !!!New!!!
	if (!rootNode)
	{
		return;
	}
	rootNode->Tick(context, blackBord, result);
}

RootNode& BehaviorTree::GetRoot()
{
	return *rootNode;
}