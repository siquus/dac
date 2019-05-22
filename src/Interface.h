/*
 * Interface.h
 *
 *  Created on: May 22, 2019
 *      Author: derommo
 */

#ifndef SRC_INTERFACE_H_
#define SRC_INTERFACE_H_

#include <fstream>

#include "GlobalDefines.h"
#include "Graph.h"
#include "Module.h"

namespace Interface {

class Output {
	std::string fileName_;
	Graph* graph_ = nullptr;
	Graph::NodeId_t nodeId_ = Graph::NODE_ID_NONE;

public:
	Output(Graph* graph, const std::string* fileName);
	bool Add(const Algebra::Module::VectorSpace::Vector * vector);
};

}
#endif /* SRC_INTERFACE_H_ */
