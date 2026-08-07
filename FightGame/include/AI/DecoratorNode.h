#pragma once

#include <memory>

#include "Node.h"

// !!!New!!!
class DecoratorNode : public Node
{
public:
	explicit DecoratorNode(std::unique_ptr<Node> child);
	~DecoratorNode() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;

protected:
	// !!!New!!!
	virtual bool CheckCondition(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result);

	std::unique_ptr<Node> childNode;

private:
	bool PrepareChildContext(AIContext& context);
};
