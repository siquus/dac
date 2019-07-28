/*
 * Parallizer.cpp
 *
 *  Created on: Jun 24, 2019
 *      Author: derommo
 */

#include <set>
#include "GlobalDefines.h"

#include "Parallizer.h"

Parallizer::Parallizer()
{
	// TODO: Read hardware from file
	cpu_t cpu;
	cpu.coresNrOf = 4;

	cpuInfo_[0] = cpu;
}

Parallizer::~Parallizer()
{

}

bool Parallizer::Parallize(Graph* graph)
{
	// TODO: Parallize among available hardware (GPU, other computers, ...)
	std::vector<Node> * nodes = graph->GetNodesModifyable();

	for(Node &node: *nodes)
	{
		// All run on the same CPU
		node.hardware.identifier = 0;
		node.hardware.type = Node::HARDWARE_TYPE_CPU;
	}

	return true;
}

const std::map<Node::HardwareIdentifier_t, Parallizer::cpu_t> * Parallizer::GetCpuInfo(void) const
{
	return &cpuInfo_;
}

