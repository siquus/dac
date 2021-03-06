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

#ifndef SRC_INTERFACE_H_
#define SRC_INTERFACE_H_

#include <fstream>
#include <map>

#include "GlobalDefines.h"
#include "Graph.h"
#include "Module.h"

namespace Interface {

class Output : public NodeRef {
	void Init(Graph * graph, const char * name);
	std::string name_;
	std::string callbackName_;

public:
	Output(Graph* graph, const std::string* name);
	Output(Graph* graph, const char * name);
	bool Set(const Algebra::Module::VectorSpace::Vector * vector);
	const std::string * GetName() const;
	const std::string * GetCallbackName() const;
};

class Input {
	std::string name_;
	std::string callbackName_;
	Graph * graph_;
	Algebra::Ring::type_t Ring_;

	std::map<Node::Id_t, size_t> inputs_; // map key is the input node

public:
	Input(Graph* graph, const char * name, Algebra::Ring::type_t ring);
	const Algebra::Module::VectorSpace::Vector * Get(const Algebra::Module::VectorSpace * vspace, size_t identifier);

	const std::string * GetName() const;
	const std::string * GetCallbackName() const;
	size_t GetIdentifier(const Node::Id_t &NodeId) const;

	static bool AreEqual(const Input * lIn, const Input * rIn);
};

}
#endif /* SRC_INTERFACE_H_ */
