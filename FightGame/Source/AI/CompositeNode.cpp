#include "AI/CompositeNode.h"

// !!!New!!!
CompositeNode::CompositeNode()
{
}

CompositeNode::~CompositeNode()
{
}

void CompositeNode::AddNode(std::unique_ptr<Node> child)
{
	// ノードを追加
	if (!child)
	{
		return;
	}
	child->SetNodePosition(hierarchy + 1, static_cast<int>(childNodes.size()));
	childNodes.push_back(std::move(child));
}

bool CompositeNode::UpdateContext(AIContext& context, int nextIndex)
{
	// 数値が不正な場合失敗を返す
	if(
		hierarchy < 0 || // 自身の階層が不正だった場合失敗
		context.nodeIndexes.size() <= hierarchy ||  // 自身までのノードが存在しなかった場合失敗
		childNodes.size() <= nextIndex // 数値が子ノードの数以上だった場合失敗
		)
	{
		return false;
	}

	// 既に数値が入っていた場合成功を返す
	if(hierarchy + 1 < context.nodeIndexes.size() &&
	   context.nodeIndexes.at(hierarchy + 1) == nextIndex)
	{
		return true;
	}

	// 自分の階層以降の情報を削除する
	while(context.nodeIndexes.size() > hierarchy + 1)
	{
		context.nodeIndexes.pop_back();
	}
	// 子ノードを追加する
	context.nodeIndexes.push_back(nextIndex);
	return true;
}