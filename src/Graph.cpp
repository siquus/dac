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

const Node * Graph::GetNode(Node::Id_t id) const
{
	for(const Node &node: nodes_)
	{
		if(node.id == id)
		{
			return &node;
		}
	}

	return nullptr;
}

std::vector<Node> * Graph::GetNodesModifyable()
{
	return &nodes_;
}

bool Graph::DeleteChildReferences(Node::Id_t child)
{
	for(Node &parent: nodes_)
	{
		for(auto childIt = parent.children.begin(); childIt != parent.children.end(); childIt++)
		{
			if(child == *childIt)
			{
				parent.children.erase(childIt);
				break;
			}
		}
	}
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

const char* Node::getName(Type type)
{
	switch(type)
	{
	case Type::VECTOR:
		return "VECTOR";

	case Type::VECTOR_ADDITION:
		return "VECTOR_ADDITION";

	case Type::VECTOR_CONTRACTION:
		return "VECTOR_CONTRACTION";

	case Type::VECTOR_SCALAR_MULTIPLICATION:
		return "VECTOR_SCALAR_MULTIPLICATION";

	case Type::VECTOR_COMPARISON_IS_SMALLER:
		return "VECTOR_COMPARISON_IS_SMALLER";

	case Type::VECTOR_KRONECKER_DELTA_PRODUCT:
		return "VECTOR_KRONECKER_DELTA_PRODUCT";

	case Type::OUTPUT:
		return "OUTPUT";

	case Type::CONTROL_TRANSFER_WHILE:
		return "CONTROL_TRANSFER_WHILE";

	default:
		Error("Unknown Type %u\n", (uint8_t) type);
		return nullptr;
	}

	// should not be reached
	return nullptr;
}

bool NodeRef::StoreIn(const NodeRef* nodeRef)
{
	if(nodeRef->graph_ != graph_)
	{
		Error("Different Graphs!\n");
		return false;
	}

	auto nodes = graph_->GetNodesModifyable();

	// TODO: Test that nodes are compatible!
	uint8_t foundCnt = 0;
	for(auto &node: *nodes)
	{
		if(node.id == nodeId_)
		{
			node.storedIn_ = nodeRef->nodeId_;
			foundCnt++;
		}
		else if(node.id == nodeRef->nodeId_)
		{
			node.usedAsStorageBy_.push_back(nodeId_);
			foundCnt++;
		}

		if(2 == foundCnt)
		{
			return true;
		}
	}

	return false;
}
