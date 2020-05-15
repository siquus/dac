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
	callbackName_ = "Dac" + graph->Name() + "OutputCallback" + name_;

	if(nullptr == graph)
	{
		Error("nullptr\n");
		return;
	}

	graph_ = graph;

	Node node(
			Node::Object_t::INTERFACE_OUTPUT, this,
			Node::Type::OUTPUT, nullptr);

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

const std::string * Output::GetName() const
{
	return &name_;
}

const std::string * Output::GetCallbackName() const
{
	return &callbackName_;
}

const std::string * Input::GetName() const
{
	return &name_;
}

Input::Input(Graph * graph, const char * name, Algebra::Ring::type_t ring)
{
	if((nullptr == graph) || (nullptr == name))
	{
		Error("nullptr\n");
		return;
	}

	graph_ = graph;
	Ring_ = ring;

	name_ = name;
	callbackName_ = "Dac" + graph->Name() + "InputCallback" + name_;
}

const std::string * Input::GetCallbackName() const
{
	return &callbackName_;
}

bool Input::AreEqual(const Input * lIn, const Input * rIn)
{
	const std::string * lName = lIn->GetCallbackName();
	const std::string * rName = rIn->GetCallbackName();

	if(lName->size() != rName->size())
	{
		return false;
	}

	if(!std::equal(lName->begin(), lName->end(), rName->begin()))
	{
		return false;
	}

	if(lIn->inputs_.size() != rIn->inputs_.size())
	{
		return false;
	}

	if(!std::equal(lIn->inputs_.begin(), lIn->inputs_.end(), rIn->inputs_.begin()))
	{
		return false;
	}

	return true;
}

size_t Input::GetIdentifier(const Node::Id_t &NodeId) const
{
	auto identifier = inputs_.find(NodeId);
	if(inputs_.end() == identifier)
	{
		Error("Could not find NodeId!\n");
		return SIZE_MAX;
	}

	return identifier->second;
}

const Algebra::Module::VectorSpace::Vector * Input::Get(const Algebra::Module::VectorSpace * vspace, size_t identifier)
{
	if(Ring_ != vspace->GetRing())
	{
		Error("Input may only return vectors over %s!\n", Algebra::Ring::GetTypeString(Ring_));
		return nullptr;
	}

	Node node(
			Node::Object_t::INTERFACE_INPUT, this,
			Node::Type::INPUT, nullptr);

	Node::Id_t inNodeId = graph_->AddNode(&node);
	if(Node::ID_NONE == inNodeId)
	{
		Error("Could not add node!\n");
		return nullptr;
	}

	inputs_.insert({inNodeId, identifier});

	Algebra::Module::VectorSpace::Vector::propertyExternalInput_t extInput = {.InputNode = inNodeId};

	auto vector = vspace->Element(graph_,
			std::map<Algebra::Module::VectorSpace::Vector::Property, const void *>{
		{Algebra::Module::VectorSpace::Vector::Property::ExternalInput, &extInput}});

	if(nullptr == vector)
	{
		Error("Could not create vector!\n");
	}

	return vector;
}
