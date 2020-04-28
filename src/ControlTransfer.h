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

#ifndef SRC_CONTROLTRANSFER_H_
#define SRC_CONTROLTRANSFER_H_

#include "GlobalDefines.h"
#include "Graph.h"
#include "Module.h"

namespace ControlTransfer {

class While : public NodeRef {
	public:
	While(Graph* graph);

	bool Set(
			const Algebra::Module::VectorSpace::Vector * condition,
			const std::vector<const NodeRef*> &parents, // These nodes need to be executed before condition is checked
			const NodeRef* trueNode, // Execute all parents and their children (including this one) of this node if condition is true
			const NodeRef* falseNode); // Execute all parents and their children (including this one) of this node if condition is false. Set null if done.
};

}
#endif /* SRC_CONTROLTRANSFER_H_ */
