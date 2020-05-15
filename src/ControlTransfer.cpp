/*
 * This file is part of
 * Distributed Algebraic Computations (https://github.com/siquus/dac)
 *
 * GPL-3 (or later)
 *
 * Copyright (C) 2020  Patrik Omland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ControlTransfer.h"

using namespace ControlTransfer;

While::While(Graph * graph)
{
	graph_ = graph;
}

bool While::Set(
		const Algebra::Module::VectorSpace::Vector * condition,
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

	if(condition->Space()->GetDim() != 1)
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
	node.Type_ = Node::Type::CONTROL_TRANSFER_WHILE;

	auto param = new Node::ControlTransferParameters_t;

	if(nullptr != falseNode)
	{
		param->BranchFalse = falseNode->nodeId_;
	}

	if(nullptr != trueNode)
	{
		param->BranchTrue = trueNode->nodeId_;
	}

	node.TypeParameters_ = param;

	nodeId_ = graph_->AddNode(&node);

	if(Node::ID_NONE == nodeId_)
	{
		Error("Could not add Node!\n");
		return false;
	}

	PushParent(condition->nodeId_);

	for(const NodeRef * nodeRef: parents)
	{
		if(nodeRef != condition) // Make sure condition is not added twice
		{
			PushParent(nodeRef->nodeId_);
		}
	}

	return true;
}
