/*
 * Parallizer.h
 *
 *  Created on: Jun 24, 2019
 *      Author: derommo
 */

#ifndef SRC_PARALLIZER_H_
#define SRC_PARALLIZER_H_

#include "Graph.h"

#include <map>

class Parallizer {
public:
	Parallizer();
	virtual ~Parallizer();

	bool Parallize(Graph* graph);

	typedef struct {
		uint16_t coresNrOf;
	} cpu_t;
	const std::map<Graph::HardwareIdentifier_t, cpu_t> * GetCpuInfo() const;

private:
	std::map<Graph::HardwareIdentifier_t, cpu_t> cpuInfo_;
};

#endif /* SRC_PARALLIZER_H_ */
