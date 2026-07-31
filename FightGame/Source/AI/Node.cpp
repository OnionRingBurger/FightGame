#include "AI/Node.h"

// !!!New!!!
Node::Node()
	: hierarchy(0)
	, nodeIndex(0)
{
}

Node::~Node()
{
}

void Node::SetNodePosition(int a_hierarchy, int a_nodeIndex)
{
	// !!!New!!!
	hierarchy = a_hierarchy;
	nodeIndex = a_nodeIndex;
}