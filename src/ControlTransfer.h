/*
 * ControlTransfer.h
 *
 *  Created on: Jul 28, 2019
 *      Author: derommo
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
