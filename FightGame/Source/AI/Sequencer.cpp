#include "AI/Sequencer.h"

// !!!New!!!
Sequencer::Sequencer() 
	: CompositeNode()
{
}

Sequencer::~Sequencer()
{
}

Node::Status Sequencer::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	// 子ノードが存在しない場合成功を返す
	if (childNodes.empty())
	{
		return NODE_SUCCES;
	}

	// 子ノードが登録されていない場合登録する
	if (context.nodeIndexes.size() <= static_cast<size_t>(hierarchy + 1))
	{
		bool isSafe = UpdateContext(context, 0);
		if (!isSafe) return NODE_FAILURE;
	}

	int currentIndex = context.nodeIndexes.at(hierarchy + 1);

	for (int i = currentIndex; i < static_cast<int>(childNodes.size()); i++)
	{
		// 子ノードの位置を更新
		bool isSafe = UpdateContext(context, i);
		if(!isSafe) return NODE_FAILURE;
		// 子ノードを更新
		Status childState = childNodes.at(i)->Tick(context, blackBord, mind, systemInfo, result);
		// 成功した場合次に行く
		if (childState == NODE_SUCCES) continue;

		// 子ノードが失敗、あるいは実行中だった場合返す
		return childState;
	}

	return NODE_SUCCES;
}