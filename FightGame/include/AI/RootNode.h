#pragma once

#include <memory>

#include "Node.h"

// !!!New!!!
// ツリー入口。子は1つのみ。Context に自身のパスを登録する。
class RootNode : public Node
{
public:
	RootNode();
	~RootNode() override;

	// 子は1つまで
	bool AddNode(std::unique_ptr<Node> child);

	Status Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result) override;

private:

	std::unique_ptr<Node> childNode;
};