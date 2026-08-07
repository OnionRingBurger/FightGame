#include "AI/Selector.h"

// !!!New!!!
Selector::Selector()
{
}

Selector::~Selector()
{
}

Node::Status Selector::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	// 子ノードが存在しなかった場合抜ける
	if (context.nodeIndexes.size() <= static_cast<size_t>(hierarchy)) return NODE_FAILURE;
	int currentIndex = context.nodeIndexes.at(hierarchy + 1);

	for (int i = currentIndex; i < static_cast<int>(childNodes.size()); i++)
	{
		// 子ノードの位置を更新
		bool isSafe = UpdateContext(context, i);
		if(!isSafe) return NODE_FAILURE;
		// 子ノードを更新
		Status childState = childNodes.at(i)->Tick(context, blackBord, mind, systemInfo, result);
		// 失敗した場合次に行く
		if (childState == NODE_FAILURE) continue;

		// 子ノードが成功、あるいは実行中だった場合返す
		return childState;
	}

	return NODE_FAILURE;
}