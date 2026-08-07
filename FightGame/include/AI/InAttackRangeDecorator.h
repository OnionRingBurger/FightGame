#pragma once

#include <memory>

#include "DecoratorNode.h"

// !!!New!!!
class InAttackRangeDecorator : public DecoratorNode
{
public:
	explicit InAttackRangeDecorator(std::unique_ptr<Node> child);
	~InAttackRangeDecorator() override;

protected:
	bool CheckCondition(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;
};
