/*
 * Interface.h
 *
 *  Created on: May 22, 2019
 *      Author: derommo
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
	std::string name_;
	std::map<Node::Id_t, std::string> outNames_;

public:
	Output(Graph* graph, const std::string* name);
	bool Add(const std::string* name, const Algebra::Module::VectorSpace::Vector * vector);
	const std::string * GetOutputName(Node::Id_t nodeId) const;
};

}
#endif /* SRC_INTERFACE_H_ */
