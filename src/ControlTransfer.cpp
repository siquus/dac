/*
 * ControlTransfer.cpp
 *
 *  Created on: Jul 28, 2019
 *      Author: derommo
 */

#include "ControlTransfer.h"

using namespace ControlTransfer;

While::While(Graph * graph)
{
	graph_ = graph;
}

bool While::Init(vector_t* condition, NodeRef* trueNode, NodeRef* falseNode)
{
	if((nullptr == graph_) || (nullptr == trueNode) || (nullptr == falseNode))
	{
		Error("nullptr\n");
		return false;
	}

	if(condition->graph_ != graph_)
	{
		Error("Condition graph does not match While Graph!\n");
		return false;
	}

	if(condition->__space_->dim_ != 1)
	{
		Error("Condition must be a scalar!\n");
		return false;
	}

	if(trueNode->graph_ != graph_)
	{
		Error("trueNode graph does not match While Graph!\n");
		return false;
	}

	if(falseNode->graph_ != graph_)
	{
		Error("falseNode graph does not match While Graph!\n");
		return false;
	}

	Node node;
	node.parents.push_back(condition->nodeId_);

	node.type = Node::Type::CONTROL_TRANSFER_WHILE;
	node.objectType = Node::ObjectType::NONE;
	node.object = nullptr;

	Node::Id_t nodeId = graph_->AddNode(&node);

	if(Node::ID_NONE == nodeId)
	{
		Error("Could not add Node!\n");
		return false;
	}

	// Add this parent to children
	bool success;
	success = graph_->AddParent(nodeId, falseNode->nodeId_);
	if(!success)
	{
		Error("Could not add Parent!");
		return false;
	}


	success = graph_->AddParent(nodeId, trueNode->nodeId_);
	if(!success)
	{
		Error("Could not add Parent!");
		return false;
	}

	return true;
}
