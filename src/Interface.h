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

class Output {
	std::string name_;
	Graph* graph_ = nullptr;
	Graph::NodeId_t nodeId_ = Graph::NODE_ID_NONE;
	std::map<Graph::NodeId_t, std::string> outNames_;

public:
	Output(Graph* graph, const std::string* name);
	bool Add(const std::string* name, const Algebra::Module::VectorSpace::Vector * vector);
};

}
#endif /* SRC_INTERFACE_H_ */
