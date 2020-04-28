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

#include "Interface.h"

using namespace Interface;

Output::Output(Graph * graph, const char * name)
{
	Init(graph, name);
}

Output::Output(Graph* graph, const std::string* name)
{
	Init(graph, name->c_str());
}

void Output::Init(Graph * graph, const char * name)
{
	if(nullptr == name)
	{
		Error("nullptr\n");
		return;
	}

	name_ = name;

	if(nullptr == graph)
	{
		Error("nullptr\n");
		return;
	}

	graph_ = graph;

	Node node;
	node.type = Node::Type::OUTPUT;
	node.objectType = Node::ObjectType::INTERFACE_OUTPUT;
	node.object = this;

	nodeId_ = graph->AddNode(&node);
	if(Node::ID_NONE == nodeId_)
	{
		Error("Could not add node!\n");
		return;
	}
}

bool Output::Set(const Algebra::Module::VectorSpace::Vector * vector)
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

	return true;
}

const std::string * Output::GetOutputName() const
{
	return &name_;
}
