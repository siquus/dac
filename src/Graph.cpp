/*
 * Graph.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#include "GlobalDefines.h"
#include "Graph.h"

const Graph::NodeId_t Graph::NODE_ID_NONE = 0;

Graph::Graph() {
	// TODO Auto-generated constructor stub

}

Graph::~Graph() {
	// TODO Auto-generated destructor stub
}

Graph::NodeId_t Graph::AddNode(Node_t * node)
{
	// Add the node
	node->id = nextNodeId_;
	nodes_.push_back(*node);

	nextNodeId_++;

	// Let parents know of its existence
	for(NodeId_t &parent: node->parents)
	{
		bool success = AddChild(parent, node->id);
		if(!success)
		{
			Error("Could not AddChild to Parents\n");
			return Graph::NODE_ID_NONE;
		}
	}

	return node->id ;
}

bool Graph::AddChild(NodeId_t parent, NodeId_t child)
{
	// Search for parent and add child
	for(Node_t &node: nodes_)
	{
		if(node.id == parent)
		{
			node.children.push_back(child);
			return true;
		}
	}

	return false;
}

