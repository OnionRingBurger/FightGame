#include "AI/RootNode.h"

// !!!New!!!
RootNode::RootNode()
{
	SetNodePosition(0, 0);
}

RootNode::~RootNode()
{
}

bool RootNode::AddNode(std::unique_ptr<Node> child)
{
	// !!!New!!!
	if (!child)
	{
		return false;
	}
	// 子は1つのみ
	if (childNode)
	{
		return false;
	}
	child->SetNodePosition(hierarchy + 1, 0);
	childNode = std::move(child);
	return true;
}

Node::Status RootNode::Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result)
{
	// !!!New!!!
	// Root のみが Context に自身(0)と最初の子カーソル(0)を登録する
	if (context.nodeIndexes.empty())
	{
		context.nodeIndexes.push_back(nodeIndex);
		context.nodeIndexes.push_back(0);
	}

	if (!childNode)
	{
		return NODE_FAILURE;
	}

	return childNode->Tick(context, blackBord, systemInfo, result);
}