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

