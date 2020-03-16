/*
 * ModuleProduct.cpp
 *
 *  Created on: Mar 16, 2020
 *      Author: derommo
 */

#include "Graph.h"
#include "Module.h"
#include "Ring.h"
#include "Interface.h"
#include "CodeGenerator.h"

#include "ModuleProduct.h"

bool ModuleProduct::Generate(const std::string &path)
{
	Graph graph("ModuleProduct");

	auto myVs = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 3);

	auto vector1_init = std::vector<float>{1, 2, 3};
	auto vector2_init = std::vector<float>{4, 5, 6};

	auto vector1 = myVs.Element(&graph, vector1_init);
	auto vector2 = myVs.Element(&graph, vector2_init);

	auto vecVecProd = vector1->Multiply(vector2);

	auto vecVecProdOutput = Interface::Output(&graph, "vecVecProd");
	vecVecProdOutput.Set(vecVecProd);

	// Generate Code

	CodeGenerator codeGenerator(&path);
	bool GenSuccess = codeGenerator.Generate(&graph);
	if(!GenSuccess)
	{
		printf("Could not generate Code\n");
		return 1;
	}

	return true;
}

