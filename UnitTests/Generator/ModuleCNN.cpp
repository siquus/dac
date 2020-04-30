/*
 * ModuleCNN.cpp
 *
 *  Created on: Apr 23, 2020
 *      Author: derommo
 */

#include <numeric>

#include "Graph.h"
#include "Module.h"
#include "Ring.h"
#include "Interface.h"
#include "CodeGenerator.h"

#include "ModuleCNN.h"

bool ModuleCNN::Generate(const std::string &path)
{
	Graph graph("ModuleCNN");

	// Generate Code
	auto inputSpace = Algebra::Module::VectorSpace(Algebra::Ring::Float32, std::vector<dimension_t>{10, 10});
	auto kernelSpace = Algebra::Module::VectorSpace(Algebra::Ring::Float32, std::vector<dimension_t>{3, 3});

	auto inputInit = std::vector<float>(10 * 10);
	std::iota(inputInit.begin(), inputInit.end(), 1);

	auto input = inputSpace.Element(&graph, inputInit);

	auto kernelInit = std::vector<float>{1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto kernel = kernelSpace.Element(&graph, kernelInit);

	auto cc = input->CrossCorrelate(kernel);
	auto ccOutput = Interface::Output(&graph, "cc");
	ccOutput.Set(cc);

	auto ccMaxPool = cc->MaxPool(std::vector<uint32_t>{2, 2});
	auto ccMaxPoolOutput = Interface::Output(&graph, "ccMaxPool");
	ccMaxPoolOutput.Set(ccMaxPool);

	CodeGenerator codeGenerator(&path);
	bool GenSuccess = codeGenerator.Generate(&graph);
	if(!GenSuccess)
	{
		printf("Could not generate Code\n");
		return 1;
	}

	return true;
}
