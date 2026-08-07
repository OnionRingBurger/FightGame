#include "AI/DecoratorNode.h"

// 子ノードをコンストラクタで受け取る
DecoratorNode::DecoratorNode(std::unique_ptr<Node> child)
	: childNode(std::move(child))
{
	// ヒエラルキーを登録
	childNode->SetNodePosition(hierarchy + 1, 0);
}

DecoratorNode::~DecoratorNode()
{
}

Node::Status DecoratorNode::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	// 子ノードが存在しなければ抜ける
	if (!childNode)
	{
		return NODE_FAILURE;
	}

	// 条件を確認し、満たしていなければ抜ける
	if (!CheckCondition(context, blackBord, mind, systemInfo, result))
	{
		return NODE_FAILURE;
	}

	// コンテキストに子ノードを登録する
	if (!PrepareChildContext(context))
	{
		return NODE_FAILURE;
	}

	return childNode->Tick(context, blackBord, mind, systemInfo, result);
}

bool DecoratorNode::CheckCondition(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	(void)context;
	(void)blackBord;
	(void)mind;
	(void)systemInfo;
	(void)result;
	return true;
}

bool DecoratorNode::PrepareChildContext(AIContext& context)
{
	// 子ノードをコンテキストに登録
	if (hierarchy < 0 || context.nodeIndexes.size() <= static_cast<size_t>(hierarchy))
	{
		return false;
	}

	if (hierarchy + 1 < static_cast<int>(context.nodeIndexes.size())
		&& context.nodeIndexes.at(hierarchy + 1) == 0)
	{
		return true;
	}

	while (static_cast<int>(context.nodeIndexes.size()) > hierarchy + 1)
	{
		context.nodeIndexes.pop_back();
	}
	context.nodeIndexes.push_back(0);
	context.actionDuration = 0.0f;
	return true;
}
