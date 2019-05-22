/*
 * Interface.cpp
 *
 *  Created on: May 22, 2019
 *      Author: derommo
 */

#include "Interface.h"

using namespace Interface;

Output::Output(Graph* graph, const std::string* fileName)
{
	if(nullptr == fileName)
	{
		Error("nullptr\n");
		return;
	}

	fileName_ = *fileName;

	if(nullptr == graph)
	{
		Error("nullptr\n");
		return;
	}

	graph_ = graph;

	Graph::Node_t node;
	node.nodeType = Graph::NodeType::OUTPUT;
	node.objectType = Graph::ObjectType::INTERFACE_OUTPUT;
	node.object = this;

	nodeId_ = graph->AddNode(&node);
	if(Graph::NODE_ID_NONE == nodeId_)
	{
		Error("Could not add node!\n");
		return;
	}
}

bool Output::Add(const Algebra::Module::VectorSpace::Vector * vector)
{
	if(nullptr == vector)
	{
		Error("Nullptr\n");
		return false;
	}

	bool success = graph_->AddParent(vector->__nodeId_, nodeId_);
	if(!success)
	{
		Error("Could not AddParent to Output!\n");
		return false;
	}

	return true;
}
