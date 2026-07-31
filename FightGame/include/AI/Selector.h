#pragma once

#include "CompositeNode.h"

// !!!New!!!
class Selector : public CompositeNode
{
public:
	Selector();
	~Selector() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIResult& result) override;
};