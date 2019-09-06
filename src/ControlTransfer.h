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
	bool Init(vector_t* condition, NodeRef* trueNode, NodeRef* falseNode);

	enum {
		CHILD_GOTO_FALSE,
		CHILD_GOTO_TRUE,
		CHILD_NROF,
	};
};

}
#endif /* SRC_CONTROLTRANSFER_H_ */
