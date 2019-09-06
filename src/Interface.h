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
	void Init(Graph * graph, const char * name);
	std::string name_;

public:
	Output(Graph* graph, const std::string* name);
	Output(Graph* graph, const char * name);
	bool Set(const Algebra::Module::VectorSpace::Vector * vector);
	const std::string * GetOutputName() const;
};

}
#endif /* SRC_INTERFACE_H_ */
