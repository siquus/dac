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
	std::vector<Graph::Node_t> * nodes = graph->GetNodesModifyable();

	for(Graph::Node_t &node: *nodes)
	{
		// All run on the same CPU
		node.hardware.identifier = 0;
		node.hardware.type = Graph::HARDWARE_TYPE_CPU;
	}

	return true;
}

const Parallizer::cpu_t * Parallizer::GetCpuInfo(Graph::HardwareIdentifier_t id)
{
	const auto cpuInfo = cpuInfo_.find(id);
	if(cpuInfo_.end() == cpuInfo)
	{
		Error("Unknown CPU Identifier %u\n", id);
		return nullptr;
	}

	return &cpuInfo->second;
}

