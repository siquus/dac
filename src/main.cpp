/*
 * main.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#include <stdio.h>

#include "Graph.h"
#include "Module.h"
#include "Ring.h"
#include "Interface.h"
#include "CodeGenerator.h"
#include "Parallizer.h"

int main()
{
	Graph graph;

	auto myVs = Algebra::Module::VectorSpace(Algebra::Ring::Float32(), 3);

	auto vec1_init = std::vector<float>{1, 2, 3};
	auto vec2_init = std::vector<float>{4, 5, 6};

	auto vec1 = myVs.Element(&graph, &vec1_init);
	auto vec2 = myVs.Element(&graph, &vec2_init);

	auto sum = vec1->Add(vec2);

	auto myScalarSpace = Algebra::Module::VectorSpace(Algebra::Ring::Float32(), 1);
	auto vec3_init = std::vector<float>{42};
	auto vec3 = myScalarSpace.Element(&graph, &vec3_init);

	auto prod = sum->Multiply(vec3);
	if(nullptr == prod)
	{
		printf("Could not create product\n");
		return 1;
	}

	auto outFile = std::string("Output");
	auto output = Interface::Output(&graph, &outFile);

	auto outName = std::string("Product");
	bool success = output.Add(&outName, prod);
	if(!success)
	{
		printf("Could not add to Output\n");
		return 1;
	}

	Parallizer parallizer;
	bool parSuccess = parallizer.Parallize(&graph);
	if(!parSuccess)
	{
		printf("Could not run parallizer\n");
		return 1;
	}

	auto outpath = std::string("");
	CodeGenerator codeGenerator(&outpath);

	bool GenSuccess = codeGenerator.Generate(&graph);
	if(!GenSuccess)
	{
		printf("Could not generate Code");
		return 1;
	}

	printf("Success!\n");
	return 0;
}


