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
		NodeRef* trueNode, bool trueNodeExeBefore,
		NodeRef* falseNode, bool falseNodeExeBefore)
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

	if(!trueNodeExeBefore)
	{
		graph_->DeleteChildReferences(trueNode->nodeId_);
	}

	if(!falseNodeExeBefore)
	{
		graph_->DeleteChildReferences(falseNode->nodeId_);
	}

	Node node;
	node.parents.push_back(condition->nodeId_);

	node.type = Node::Type::CONTROL_TRANSFER_WHILE;
	node.objectType = Node::ObjectType::CONTROL_TRANSFER_WHILE;
	node.object = this;

	Node::Id_t nodeId = graph_->AddNode(&node);

	if(Node::ID_NONE == nodeId)
	{
		Error("Could not add Node!\n");
		return false;
	}

	falseNode_ = falseNode->nodeId_;
	trueNode_ = trueNode->nodeId_;

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
