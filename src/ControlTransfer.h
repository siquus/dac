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

	typedef const Algebra::Module::VectorSpace::Vector vector_t;
	bool Set(vector_t* condition,
			NodeRef* trueNode, bool trueNodeExeBefore,
			NodeRef* falseNode, bool falseNodeExeBefore);

	Node::Id_t getTrueNode() const;
	Node::Id_t getFalseNode() const;

	private:
	Node::Id_t trueNode_ = Node::ID_NONE;
	Node::Id_t falseNode_ = Node::ID_NONE;
};

}
#endif /* SRC_CONTROLTRANSFER_H_ */
