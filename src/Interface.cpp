/*
 * Interface.cpp
 *
 *  Created on: May 22, 2019
 *      Author: derommo
 */

#include "Interface.h"

using namespace Interface;

Output::Output(Graph* graph, const std::string* name)
{
	if(nullptr == name)
	{
		Error("nullptr\n");
		return;
	}

	name_ = *name;

	if(nullptr == graph)
	{
		Error("nullptr\n");
		return;
	}

	graph_ = graph;

	Node node;
	node.Type = Node::Type::OUTPUT;
	node.objectType = Node::ObjectType::INTERFACE_OUTPUT;
	node.object = this;

	nodeId_ = graph->AddNode(&node);
	if(Node::ID_NONE == nodeId_)
	{
		Error("Could not add node!\n");
		return;
	}
}

bool Output::Add(const std::string* name, const Algebra::Module::VectorSpace::Vector * vector)
{
	if(nullptr == vector)
	{
		Error("Nullptr\n");
		return false;
	}

	if(graph_ != vector->graph_)
	{
		Error("Not on same Graph");
		return false;
	}

	bool success = graph_->AddParent(vector->nodeId_, nodeId_);
	if(!success)
	{
		Error("Could not AddParent to Output!\n");
		return false;
	}

	outNames_[vector->nodeId_] = *name;

	return true;
}

const std::string * Output::GetOutputName(Node::Id_t nodeId) const
{
	auto it = outNames_.find(nodeId);
	if(outNames_.end() == it)
	{
		return nullptr;
	}

	return &it->second;
}
