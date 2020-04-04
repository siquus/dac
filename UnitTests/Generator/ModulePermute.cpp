/*
 * ModulePermute.cpp
 *
 *  Created on: Feb 29, 2020
 *      Author: derommo
 */

#include "Graph.h"
#include "Module.h"
#include "Ring.h"
#include "Interface.h"
#include "CodeGenerator.h"

#include "ModulePermute.h"

bool ModulePermute::Generate(const std::string &path)
{
	Graph graph("ModulePermute");

	auto myVs = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 3);

	// Matrix transpose
	auto myMatrixSpace = Algebra::Module::VectorSpace(myVs, 2);

	auto matrix_init = std::vector<float>{1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto matrix = myMatrixSpace.Element(&graph, matrix_init);

	auto matrixTranspose = matrix->Permute(std::vector<uint32_t>{1, 0});

	auto matrixTransposeOutput = Interface::Output(&graph, "matrixTranspose");
	matrixTransposeOutput.Set(matrixTranspose);

	// Tensor Reordering
	auto myTensorSpace = Algebra::Module::VectorSpace({&myVs, &myVs, &myVs});

	auto tensor_init = std::vector<float>{
		1, 2, 3, 4, 5, 6, 7, 8, 9,
		10 , 11, 12, 13, 14, 15, 16, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28};

	auto tensor = myTensorSpace.Element(&graph, tensor_init);

	auto tensorPermute = tensor->Permute(std::vector<uint32_t>{2, 1, 0});

	auto tensorPermuteOutput = Interface::Output(&graph, "tensorPermute");
	tensorPermuteOutput.Set(tensorPermute);

	// Permute derivative
	auto dMatrixTranspose = matrixTranspose->Derivative(matrix);

	// Contract it with matrix so it's easier to test
	// Since transposition is linear, we should get matrix transpose back from this
	auto dMatrixTransposeContracted = dMatrixTranspose->Contract(
			matrix,
			std::vector<uint32_t>{0, 1},
			std::vector<uint32_t>{0, 1});

	auto dMatrixTransposeContractedOutput = Interface::Output(&graph, "dMatrixTransposeContracted");
	dMatrixTransposeContractedOutput.Set(dMatrixTransposeContracted);

	// Project // TODO: Projection tests inside permutation is wrong.
	auto vectorSpace = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 9);

	auto vector_init = std::vector<float>{1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto vector = vectorSpace.Element(&graph, vector_init);

	auto projVector = vector->Project(std::pair<uint32_t, uint32_t>{0, 3});
	auto projVectorOutput = Interface::Output(&graph, "projVector");
	projVectorOutput.Set(projVector);

	auto dProjVector = projVector->Derivative(vector);
	auto dProjVectorOutput = Interface::Output(&graph, "dProjVector");
	dProjVectorOutput.Set(dProjVector);

	// Project derivative

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

