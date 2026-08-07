#pragma once

#include "CompositeNode.h"

// !!!New!!!
class ReactiveSelector : public CompositeNode
{
public:
	ReactiveSelector();
	~ReactiveSelector() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;
};
