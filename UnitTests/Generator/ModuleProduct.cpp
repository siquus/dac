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

	auto myScalarSpace = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 1);
	auto myVs = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 3);
	auto myMatrixSpace = Algebra::Module::VectorSpace(myVs, 2);

	auto scalar_init = std::vector<float>{42};
	auto vector1_init = std::vector<float>{1, 2, 3};
	auto vector2_init = std::vector<float>{4, 5, 6};

	auto scalar = myScalarSpace.Element(&graph, scalar_init);
	auto vector1 = myVs.Element(&graph, vector1_init);
	auto vector2 = myVs.Element(&graph, vector2_init);

	// Scalar - Vector Multiplication
	auto scalarVecProd = scalar->Multiply(vector1);
	auto scalarVecProdOutput = Interface::Output(&graph, "scalarVecProd");
	scalarVecProdOutput.Set(scalarVecProd);

	auto vecScalarProd = vector1->Multiply(scalar);
	auto vecScalarProdOutput = Interface::Output(&graph, "vecScalarProd");
	vecScalarProdOutput.Set(vecScalarProd);

	// Form 2-Tensor by applying tensor product to two vectors
	auto vecVecProd = vector1->Multiply(vector2);

	auto vecVecProdOutput = Interface::Output(&graph, "vecVecProd");
	vecVecProdOutput.Set(vecVecProd);

	// Try the same with delta_ij
	Algebra::Module::VectorSpace::KroneckerDeltaParameters_t deltaKronParam = {
			.DeltaPair{1, 0},
			.Scaling = 2,
	};
	auto delta_ij = myMatrixSpace.Element(&graph, deltaKronParam);

	auto vecKronProd = vector1->Multiply(delta_ij);
	auto vecKronProdContr = delta_ij->Contract(vecKronProd, std::vector<uint32_t>{0, 1}, std::vector<uint32_t>{0, 1});

	auto vecKronProdOutput = Interface::Output(&graph, "vecKronProd");
	vecKronProdOutput.Set(vecKronProdContr);

	auto kronVecProd = delta_ij->Multiply(vector1);
	auto kronVecProdContr = delta_ij->Contract(kronVecProd, std::vector<uint32_t>{0, 1}, std::vector<uint32_t>{0, 1});

	auto kronVecProdOutput = Interface::Output(&graph, "kronVecProd");
	kronVecProdOutput.Set(kronVecProdContr);

	// Take derivative
	auto dVecVecProdLeft = vecVecProd->Derivative(vector1);

	// Contract with delta_ij to make it easier to check

	auto dVecVecProdLeftContracted = delta_ij->Contract(dVecVecProdLeft, std::vector<uint32_t>{0, 1}, std::vector<uint32_t>{0, 1});

	auto dVecVecProdLeftOutput = Interface::Output(&graph, "dVecVecProdLeft");
	dVecVecProdLeftOutput.Set(dVecVecProdLeftContracted);


	auto dVecVecProdRight = vecVecProd->Derivative(vector2);

	auto dVecVecProdRightContracted = delta_ij->Contract(dVecVecProdRight, std::vector<uint32_t>{0, 1}, std::vector<uint32_t>{0, 1});

	auto dVecVecProdRightOutput = Interface::Output(&graph, "dVecVecProdRight");
	dVecVecProdRightOutput.Set(dVecVecProdRightContracted);

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

