#pragma once

#include "Node.h"

// !!!New!!!
// 葉ノード(子を持たない)
class LeafNode : public Node
{
public:
	LeafNode();
	~LeafNode() override;
};