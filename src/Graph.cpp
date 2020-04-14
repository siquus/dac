/*
 * Graph.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#include "GlobalDefines.h"
#include "Graph.h"

const std::string &Graph::Name() const
{
	return name_;
}

void Graph::Init(const std::string &name)
{
	name_ = name;
}

Graph::Graph(const std::string &name) {
	Init(name);
}

Graph::Graph(const char * name) {
	Init(std::string(name));
}

Graph::~Graph() {
	// TODO Auto-generated destructor stub
}

Node::Id_t Graph::AddNode(Node * node)
{
	// Add the node
	node->id = nextNodeId_;
	nodes_.insert(std::pair<Node::Id_t, Node>{node->id, *node});

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
	auto parentNode = nodes_.find(parent);
	if(nodes_.end() == parentNode)
	{
		Error("Could not find parent!\n");
		return false;
	}

	parentNode->second.children.push_back(child);

	return true;
}

const std::map<Node::Id_t, Node> * Graph::GetNodes() const
{
	return &nodes_;
}

const Node * Graph::GetNode(Node::Id_t id) const
{
	auto node = nodes_.find(id);
	if(nodes_.end() == node)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	return &node->second;
}

std::map<Node::Id_t, Node> * Graph::GetNodesModifyable()
{
	return &nodes_;
}

bool Graph::DeleteChildReferences(Node::Id_t child)
{
	for(auto &parentPair: nodes_)
	{
		for(auto childIt = parentPair.second.children.begin(); childIt != parentPair.second.children.end(); childIt++)
		{
			if(child == *childIt)
			{
				parentPair.second.children.erase(childIt);
				break;
			}
		}
	}

	return true;
}

bool Graph::AddParent(Node::Id_t parent, Node::Id_t child)
{
	// search for Child and add parent
	auto childPair = nodes_.find(child);
	if(nodes_.end() == childPair)
	{
		Error("Could not find node!\n");
		return false;
	}

	childPair->second.parents.push_back(parent);

	return AddChild(parent, child);
}

const char * Node::getName() const
{
	return getName(type);
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

	case Type::VECTOR_SCALAR_PRODUCT:
		return "VECTOR_SCALAR_PRODUCT";

	case Type::VECTOR_VECTOR_PRODUCT:
		return "VECTOR_VECTOR_PRODUCT";

	case Type::VECTOR_POWER:
		return "VECTOR_POWER";

	case Type::VECTOR_COMPARISON_IS_SMALLER:
		return "VECTOR_COMPARISON_IS_SMALLER";

	case Type::VECTOR_KRONECKER_DELTA_PRODUCT:
		return "VECTOR_KRONECKER_DELTA_PRODUCT";

	case Type::VECTOR_PERMUTATION:
		return "VECTOR_PERMUTATION";

	case Type::VECTOR_PROJECTION:
		return "VECTOR_PROJECTION";

	case Type::VECTOR_JOIN_INDICES:
		return "VECTOR_JOIN_INDICES";

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

bool Graph::GetRootAncestors(std::set<Node::Id_t> * rootParents, Node::Id_t child) const
{
	const Node * childNode = GetNode(child);
	if(nullptr == childNode)
	{
		Error("Could not find child node!\n");
		return false;
	}

	if(0 == childNode->parents.size())
	{
		rootParents->insert(childNode->id);
		return true;
	}

	for(const auto &parentId: childNode->parents)
	{
		if(false == GetRootAncestors(rootParents, parentId))
		{
			return false;
		}
	}

	return true;
}

bool NodeRef::StoreIn(const NodeRef* nodeRef) const
{
	if(nodeRef->graph_ != graph_)
	{
		Error("Different Graphs!\n");
		return false;
	}

	auto nodes = graph_->GetNodesModifyable();

	// TODO: Test that nodes are compatible!
	auto thisNode = nodes->find(nodeId_);
	if(nodes->end() == thisNode)
	{
		Error("Could not find node!\n");
		return false;
	}

	thisNode->second.storedIn_ = nodeRef->nodeId_;

	auto storageNode = nodes->find(nodeRef->nodeId_);
	if(nodes->end() == storageNode)
	{
		Error("Could not find node!\n");
		return false;
	}

	storageNode->second.usedAsStorageBy_.push_back(nodeId_);

	return true;
}
