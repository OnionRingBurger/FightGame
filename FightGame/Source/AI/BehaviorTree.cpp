#include "AI/BehaviorTree.h"

// !!!New!!!
BehaviorTree::BehaviorTree()
	: rootNode(std::make_unique<RootNode>())
{
}

BehaviorTree::~BehaviorTree()
{
}

// ツリーの更新
void BehaviorTree::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result, float deltaTime)
{
	// ノードがなければ抜ける
	if (!rootNode)
	{
		return;
	}

	// 更新
	if (rootNode->Tick(context, blackBord, mind, systemInfo, result, deltaTime) != Node::NODE_RUNNING)
	{
		context.actionDuration = 0.0f;
		context.nodeIndexes.clear();
	}
}

RootNode& BehaviorTree::GetRoot()
{
	return *rootNode;
}