#pragma once

#include "AIData.h"

// ビヘイビアツリーで使用されるノード
class Node
{
public:
	enum Status
	{
		NODE_SUCCES,
		NODE_FAILURE,
		NODE_RUNNING,
	};

	Node();
	virtual ~Node();

	// 親にノード番号を設定してもらうための関数
	void SetNodePosition(int hierarchy, int nodeIndex);

	// 処理を進める
	virtual Status Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result) = 0;

protected:

	// ノードの階層
	int hierarchy;
	// 親ノードから見た番号
	int nodeIndex;
};