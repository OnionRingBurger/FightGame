#pragma once

#include "CompositeNode.h"

// !!!New!!!
class Sequencer : public CompositeNode
{
public:
	Sequencer();
	~Sequencer() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, const AISystemInfo& systemInfo, AIResult& result) override;
};