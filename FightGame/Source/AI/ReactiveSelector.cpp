#include "AI/ReactiveSelector.h"

#include <vector>

// !!!New!!!
ReactiveSelector::ReactiveSelector()
{
}

ReactiveSelector::~ReactiveSelector()
{
}

Node::Status ReactiveSelector::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	// !!!New!!!
	if (context.nodeIndexes.size() <= static_cast<size_t>(hierarchy))
	{
		return NODE_FAILURE;
	}

	std::vector<int> savedTail;
	for (size_t idx = static_cast<size_t>(hierarchy) + 1; idx < context.nodeIndexes.size(); ++idx)
	{
		savedTail.push_back(context.nodeIndexes[idx]);
	}
	const float savedDuration = context.actionDuration;

	// 前回実行されていた子ノードの位置を取得する
	const bool hasChildCursor = context.nodeIndexes.size() > static_cast<size_t>(hierarchy) + 1;
	const int oldRunningIndex = hasChildCursor ? context.nodeIndexes[hierarchy + 1] : -1;

	for (int i = 0; i < static_cast<int>(childNodes.size()); i++)
	{
		if(i == oldRunningIndex)
		{
			// 前回の続きの処理だった場合状態を復旧させる
			context.actionDuration = savedDuration;
			while (context.nodeIndexes.size() > static_cast<size_t>(hierarchy) + 1)
			{
				// 自身の位置までContextを消す
				context.nodeIndexes.pop_back();
			}
			for (int nodeIdx : savedTail)
			{
				context.nodeIndexes.push_back(nodeIdx);
			}
		}
		bool isSafe = UpdateContext(context, i);
		if (!isSafe)
		{
			return NODE_FAILURE;
		}

		Status childState = childNodes.at(i)->Tick(context, blackBord, mind, systemInfo, result);
		if (childState == NODE_FAILURE)
		{
			continue;
		}

		return childState;
	}

	return NODE_FAILURE;
}
