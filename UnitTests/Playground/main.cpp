/*
 * main.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Graph.h"
#include "Module.h"
#include "Ring.h"
#include "Interface.h"
#include "CodeGenerator.h"
#include "Parallizer.h"

int main()
{
	Graph graph;

	auto myVs = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 3);

	auto vec1_init = std::vector<float>{1, 2, 3};
	auto vec2_init = std::vector<float>{4, 5, 6};

	auto vec1 = myVs.Element(&graph, &vec1_init);
	auto vec2 = myVs.Element(&graph, &vec2_init);

	auto sum = vec1->Add(vec2); // {5, 7, 9}

	auto myScalarSpace = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 1);
	auto vec3_init = std::vector<float>{42};
	auto vec3 = myScalarSpace.Element(&graph, &vec3_init);

	auto vec4_init = std::vector<float>{10, 20, 30};
	auto vec5_init = std::vector<float>{1, 1, 1};

	auto vec4 = myVs.Element(&graph, &vec4_init);
	auto vec5 = myVs.Element(&graph, &vec5_init);

	auto sum45 = vec4->Add(vec5); // {11, 21, 31}

	auto prod = sum->Multiply(vec3); // {210, 294, 378}
	if(nullptr == prod)
	{
		printf("Could not create product\n");
		return 1;
	}

	auto sum2 = sum45->Add(prod); // {221, 315, 409}

	auto isSmaller = sum2->IsSmaller(prod); // false

	auto outFile = std::string("Output");
	auto output = Interface::Output(&graph, &outFile);

	auto outName = std::string("Product");
	bool success = output.Add(&outName, prod);
	if(!success)
	{
		printf("Could not add to Output\n");
		return 1;
	}

	auto outName2 = std::string("Sum");
	success = output.Add(&outName2, sum2);
	if(!success)
	{
		printf("Could not add to Output2\n");
		return 1;
	}

	auto outName3 = std::string("SmallerThan");
	success = output.Add(&outName3, isSmaller);
	if(!success)
	{
		printf("Could not add to Output3\n");
		return 1;
	}

	Parallizer parallizer;
	bool parSuccess = parallizer.Parallize(&graph);
	if(!parSuccess)
	{
		printf("Could not run parallizer\n");
		return 1;
	}

	struct stat stCodePath = {0};

	char path[] = "Program/dac";
	// Test if folder already exsists
	if (stat(path, &stCodePath) == -1)
	{
	    mkdir(path, 0700);
	}

	auto outpath = std::string(path) + "/";
	CodeGenerator codeGenerator(&outpath);

	bool GenSuccess = codeGenerator.Generate(&graph, &parallizer);
	if(!GenSuccess)
	{
		printf("Could not generate Code\n");
		return 1;
	}

	printf("Success!\n");
	return 0;
}


