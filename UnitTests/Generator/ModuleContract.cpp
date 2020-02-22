/*
 * ModuleContract.cpp
 *
 *  Created on: Feb 9, 2020
 *      Author: derommo
 */

#include "Graph.h"
#include "Module.h"
#include "Ring.h"
#include "Interface.h"
#include "CodeGenerator.h"

#include "ModuleContract.h"

bool ModuleContract::Generate(const std::string &path)
{
	Graph graph("ModuleContract");

	auto myVs = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 3);
	auto myMatrixSpace = Algebra::Module::VectorSpace({&myVs, &myVs});

	auto matrix1_init = std::vector<float>{1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto matrix1 = myMatrixSpace.Element(&graph, matrix1_init);

	// Matrix times vector
	auto vector_init = std::vector<float>{1, 2, 3};
	auto vector = myVs.Element(&graph, vector_init);

	auto matrixVecProd = matrix1->Contract(vector, 1, 0);

	auto matrixVecProdOutput = Interface::Output(&graph, "matrixVecProd");
	matrixVecProdOutput.Set(matrixVecProd);

	// Vector times matrix
	auto vecMatrixProd = vector->Contract(matrix1, 0, 1);

	auto vecMatrixProdOutput = Interface::Output(&graph, "vecMatrixProd");
	vecMatrixProdOutput.Set(vecMatrixProd);

	// Matrix times identity Matrix
	auto matrix2_init = std::vector<float>{1, 0, 0, 0, 1, 0, 0, 0, 1};

	auto matrix2 = myMatrixSpace.Element(&graph, matrix2_init);

	auto matrixProd = matrix1->Contract(matrix2, 1, 0);

	auto matrixProdOutput = Interface::Output(&graph, "matrixProd1");
	matrixProdOutput.Set(matrixProd);

	// 3-Tensor and Vector
	auto myTensorSpace = Algebra::Module::VectorSpace({&myVs, &myVs, &myVs});

	auto tensor_init = std::vector<float>{
		1, 2, 3, 4, 5, 6, 7, 8, 9,
		10 , 11, 12, 13, 14, 15, 16, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28};

	auto tensor = myTensorSpace.Element(&graph, tensor_init);

	auto tensorVecContr2 = tensor->Contract(vector, 2, 0);

	auto tensorVecContr2Output = Interface::Output(&graph, "tensorVecContr2");
	tensorVecContr2Output.Set(tensorVecContr2);

	auto tensorVecContr1 = tensor->Contract(vector, 1, 0);

	auto tensorVecContr1Output = Interface::Output(&graph, "tensorVecContr1");
	tensorVecContr1Output.Set(tensorVecContr1);

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



