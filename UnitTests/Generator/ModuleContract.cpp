/*
 * This file is part of
 * Distributed Algebraic Computations (https://github.com/siquus/dac)
 *
 * GPL-3 (or later)
 *
 * Copyright (C) 2020  Patrik Omland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
	auto myMatrixSpace = Algebra::Module::VectorSpace(myVs ,2);

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

	// 3-Tensor and Matrix
	// Single Index
	auto tensorMatrixContr1 = tensor->Contract(matrix1, 1, 0);

	auto tensorMatrixContr1Output = Interface::Output(&graph, "tensorMatrixContr1");
	tensorMatrixContr1Output.Set(tensorMatrixContr1);

	// Multi Index
	auto tensorMatrixContr12 = tensor->Contract(matrix1,
			std::vector<uint32_t>{1, 2},
			std::vector<uint32_t>{0, 1});

	auto tensorMatrixContr12Output = Interface::Output(&graph, "tensorMatrixContr12");
	tensorMatrixContr12Output.Set(tensorMatrixContr12);

	// Kronecker
	auto TwoDelta_ij = myMatrixSpace.Element(&graph, std::vector<uint32_t>{1, 0}, 2.);

	auto matrixIdProd = matrix1->Contract(TwoDelta_ij, 1, 0);

	auto matrixIdProdOutput = Interface::Output(&graph, "matrixIdProd");
	matrixIdProdOutput.Set(matrixIdProd);

	auto twoMatrixTrace = matrix1->Contract(TwoDelta_ij,
			std::vector<uint32_t>{0, 1},
			std::vector<uint32_t>{0, 1});

	auto twoMatrixTraceOutput = Interface::Output(&graph, "twoMatrixTrace");
	twoMatrixTraceOutput.Set(twoMatrixTrace);

	// Derivation

	// Matrix Product
	auto delta_ij = myMatrixSpace.Element(&graph, std::vector<uint32_t>{1, 0}, 1.);

	auto dMatrixProdRight = matrixProd->Derivative(matrix2);

	// The result will be C_ijkl = A_ki delta_jl, we want to make the test easy
	// So we contract delta_jl away to get back A_ki * delta_jl * delta_jl = A_ki * 3, so 3 * Transpose(A)
	auto matrixProdRight = dMatrixProdRight->Contract(delta_ij, std::vector<uint32_t>{1, 3}, std::vector<uint32_t>{0, 1});

	auto matrixProdRightOutput = Interface::Output(&graph, "matrixProdRight");
	matrixProdRightOutput.Set(matrixProdRight);

	auto dMatrixProdLeft = matrixProd->Derivative(matrix1);
	// Here we have C_ijkl = B_jl delta_ik
	auto matrixProdLeft = dMatrixProdLeft->Contract(delta_ij, std::vector<uint32_t>{0, 2}, std::vector<uint32_t>{0, 1});

	auto matrixProdLeftOutput = Interface::Output(&graph, "matrixProdLeft");
	matrixProdLeftOutput.Set(matrixProdLeft);

	// Generate Code

	CodeGenerator codeGenerator(&path);
	bool GenSuccess = codeGenerator.Generate(&graph);
	if(!GenSuccess)
	{
		printf("Could not generate Code\n");
		return false;
	}

	return true;
}



