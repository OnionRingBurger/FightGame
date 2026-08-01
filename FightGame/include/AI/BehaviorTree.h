#pragma once

#include <memory>

#include "AIData.h"
#include "RootNode.h"

// !!!New!!!
class BehaviorTree
{
public:
	BehaviorTree();
	~BehaviorTree();

	void Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result);

	RootNode& GetRoot();

private:
	std::unique_ptr<RootNode> rootNode;
};