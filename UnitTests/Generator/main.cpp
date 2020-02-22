/*
 * main.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

#include "ControlTransfer.h"
#include "Graph.h"
#include "Module.h"
#include "Ring.h"
#include "Interface.h"
#include "CodeGenerator.h"

#include "ModuleContract.h"

#include "main.h"

int main()
{
	struct stat stCodePath = {0};
	const char path[] = "../Executor/dac";
	// Test if folder already exsists
	if (stat(path, &stCodePath) == -1)
	{
		mkdir(path, 0700);
	}

	auto outpath = std::string(path) + "/";

	ModuleContract moduleContract;
	moduleContract.Generate(outpath);

	Graph graph("Test");

	auto myVs = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 3);

	auto myMatrixSpace = Algebra::Module::VectorSpace({&myVs, &myVs});
	auto matrix1_init = std::vector<float>{1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto matrix2_init = std::vector<float>{1, 0, 0, 0, 1, 0, 0, 0, 1};

	auto matrix1 = myMatrixSpace.Element(&graph, matrix1_init);
	auto matrix2 = myMatrixSpace.Element(&graph, matrix2_init);

	auto matrixProd = matrix1->Contract(matrix2, 1, 0);

	auto matrixProdOutput = Interface::Output(&graph, "matrixProd");
	matrixProdOutput.Set(matrixProd);

	auto kron_init = Algebra::Module::VectorSpace::KroneckerDeltaParameters_t{.DeltaPair{1, 0}, .Scaling = 2};
	auto kron = myMatrixSpace.Element(&graph, kron_init);

	auto matrixKronProd = matrix1->Contract(kron, 0, 1);

	auto matrixKronProdOutput = Interface::Output(&graph, "matrixKronProd");
	matrixKronProdOutput.Set(matrixKronProd);

	auto vec1_init = std::vector<float>{1, 2, 3};
	auto vec2_init = std::vector<float>{4, 5, 6};

	auto vec1 = myVs.Element(&graph, vec1_init);
	auto vec2 = myVs.Element(&graph, vec2_init);

	auto sum = vec1->Add(vec2); // {5, 7, 9}

	auto myScalarSpace = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 1);
	auto vec3_init = std::vector<float>{42};
	auto vec3 = myScalarSpace.Element(&graph, vec3_init);

	auto vec4_init = std::vector<float>{10, 20, 30};
	auto vec5_init = std::vector<float>{1, 1, 1};

	auto vec4 = myVs.Element(&graph, vec4_init);
	auto vec5 = myVs.Element(&graph, vec5_init);

	auto sum45 = vec4->Add(vec5); // {11, 21, 31}

	auto prod = sum->Multiply(vec3); // {210, 294, 378}
	if(nullptr == prod)
	{
		printf("Could not create product\n");
		return 1;
	}

	auto sum2 = sum45->Add(prod); // {221, 315, 409}

	auto dsum2dvec4 = sum2->Derivative(vec4); // (d / dvec4) (vec4 + vec5 + (vec1 + vec2) * vec3)

	auto contr = sum45->Contract(prod); // 20202

	auto isSmaller = sum2->IsSmaller(prod); // false

	auto prodOutput = Interface::Output(&graph, "Product");
	bool success = prodOutput.Set(prod);
	if(!success)
	{
		printf("Could not set prodOutput\n");
		return 1;
	}

	auto sumOutput = Interface::Output(&graph, "Sum");
	success = sumOutput.Set(sum2);
	if(!success)
	{
		printf("Could not set sumOutput\n");
		return 1;
	}

	auto smallerThanOutput = Interface::Output(&graph, "SmallerThan");
	success = smallerThanOutput.Set(isSmaller);
	if(!success)
	{
		printf("Could not set smallerThanOutput\n");
		return 1;
	}

	// Loop test
	auto vec6_init = std::vector<float>{10, 10, 10};
	auto vec7_init = std::vector<float>{1, 1, 1};
	auto vec8_init = std::vector<float>{2, 2, 2};
	auto vec9_init = std::vector<float>{-1, -1, -1};

	auto vec6 = myVs.Element(&graph, vec6_init);
	auto vec7 = myVs.Element(&graph, vec7_init);
	auto vec8 = myVs.Element(&graph, vec8_init);
	auto vec9 = myVs.Element(&graph, vec9_init);

	auto add69 = vec6->Add(vec9); // TODO: Add ->Self->Add()
	add69->StoreIn(vec6);

	auto cond = add69->IsSmaller(vec7); // TODO: Now it can't find variable for add69..

	auto add78 = vec7->Add(vec8);

	ControlTransfer::While While(&graph);
	if(!While.Set(cond, add78, false, add69, true))
	{
		printf("Could not create While!\n");
		return 1;
	}

	auto whileOutput = Interface::Output(&graph, "While");
	success = whileOutput.Set(add69);
	if(!success)
	{
		printf("Could not add to whileOutput\n");
		return 1;
	}

	CodeGenerator codeGenerator(&outpath);

	bool GenSuccess = codeGenerator.Generate(&graph);
	if(!GenSuccess)
	{
		printf("Could not generate Code\n");
		return 1;
	}

	printf("Success!\n");
	return 0;
}

