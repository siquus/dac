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

bool While::Set(
		vector_t* condition,
		const std::vector<const NodeRef*> &parents,
		const NodeRef* trueNode,
		const NodeRef* falseNode)
{
	if((nullptr == graph_) || (nullptr == condition))
	{
		Error("nullptr\n");
		return false;
	}

	if(condition->graph_ != graph_)
	{
		Error("Condition graph does not match While Graph!\n");
		return false;
	}

	if(condition->__space_->GetDim() != 1)
	{
		Error("Condition must be a scalar!\n");
		return false;
	}

	if((nullptr != trueNode) && (trueNode->graph_ != graph_))
	{
		Error("trueNode graph does not match While Graph!\n");
		return false;
	}

	if((nullptr != falseNode) && (falseNode->graph_ != graph_))
	{
		Error("falseNode graph does not match While Graph!\n");
		return false;
	}

	Node node;
	node.parents.push_back(condition->nodeId_);

	for(const NodeRef * nodeRef: parents)
	{
		if(nodeRef != condition) // Make sure condition is not added twice
		{
			node.parents.push_back(nodeRef->nodeId_);
		}
	}

	node.type = Node::Type::CONTROL_TRANSFER_WHILE;
	node.objectType = Node::ObjectType::CONTROL_TRANSFER_WHILE;
	node.object = this;

	Node::Id_t nodeId = graph_->AddNode(&node);

	if(Node::ID_NONE == nodeId)
	{
		Error("Could not add Node!\n");
		return false;
	}

	if(nullptr != falseNode)
	{
		falseNode_ = falseNode->nodeId_;
	}

	if(nullptr != trueNode)
	{
		trueNode_ = trueNode->nodeId_;
	}

	return true;
}

Node::Id_t While::getTrueNode() const
{
	return trueNode_;
}

Node::Id_t While::getFalseNode() const
{
	return falseNode_;
}
