/*
 * Graph.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#include "GlobalDefines.h"
#include "Graph.h"

Graph::Graph() {
	// TODO Auto-generated constructor stub

}

Graph::~Graph() {
	// TODO Auto-generated destructor stub
}

Node::Id_t Graph::AddNode(Node * node)
{
	// Add the node
	node->id = nextNodeId_;
	nodes_.push_back(*node);

	nextNodeId_++;

	// Let parents know of its existence
	for(Node::Id_t &parent: node->parents)
	{
		bool success = AddChild(parent, node->id);
		if(!success)
		{
			Error("Could not AddChild to Parents\n");
			return Node::ID_NONE;
		}
	}

	return node->id ;
}

bool Graph::AddChild(Node::Id_t parent, Node::Id_t child)
{
	// Search for parent and add child
	for(Node &node: nodes_)
	{
		if(node.id == parent)
		{
			node.children.push_back(child);
			return true;
		}
	}

	Error("Could not AddChild!\n");
	return false;
}

const std::vector<Node> * Graph::GetNodes() const
{
	return &nodes_;
}

std::vector<Node> * Graph::GetNodesModifyable()
{
	return &nodes_;
}

bool Graph::AddParent(Node::Id_t parent, Node::Id_t child)
{
	// search for Child and add parent
	for(Node &node: nodes_)
	{
		if(node.id == child)
		{
			node.parents.push_back(parent);
			return AddChild(parent, child);
		}
	}

	Error("Could not AddParent!\n");
	return false;
}
