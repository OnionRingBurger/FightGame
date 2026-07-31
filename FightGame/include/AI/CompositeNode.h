#pragma once

#include <memory>
#include <vector>

#include "Node.h"

// !!!New!!!
// 子を持てる論理ノード(Composite)
class CompositeNode : public Node
{
public:
	CompositeNode();
	~CompositeNode() override;

	// 子ノードを追加する
	void AddNode(std::unique_ptr<Node> child);

protected:

	bool UpdateContext(AIContext& context, int nextIndex);

	// 子ノード
	std::vector<std::unique_ptr<Node>> childNodes;
};