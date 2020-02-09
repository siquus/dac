/*
* Distributed Algebraic Computations
* Copyright (C) 2019  Patrik Omland
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
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* This program was generated by DAC "Distributed Algebraic Computations"
* The source code for the generator is available on GitHub
* https://github.com/siquus/dac
*/


#include <stdint.h>

#include "DacTest.h"
#include "Nodes.h"

#include "Helpers.h"

#include "InstructionsTest.h"

extern DacOutputCallbackmatrixProd_t DacOutputCallbackmatrixProd;
extern DacOutputCallbackmatrixKronProd_t DacOutputCallbackmatrixKronProd;
extern DacOutputCallbackProduct_t DacOutputCallbackProduct;
extern DacOutputCallbackSum_t DacOutputCallbackSum;
extern DacOutputCallbackSmallerThan_t DacOutputCallbackSmallerThan;
extern DacOutputCallbackWhile_t DacOutputCallbackWhile;

static const float Node1[9] = {1.000000, 2.000000, 3.000000, 4.000000, 5.000000, 6.000000, 7.000000, 8.000000, 9.000000};
static const float Node2[9] = {1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000};
static const float Node8[3] = {1.000000, 2.000000, 3.000000};
static const float Node9[3] = {4.000000, 5.000000, 6.000000};
static const float Node11 = 42.000000;
static const float Node12[3] = {10.000000, 20.000000, 30.000000};
static const float Node13[3] = {1.000000, 1.000000, 1.000000};
static const float Node26[3] = {1.000000, 1.000000, 1.000000};
static const float Node27[3] = {2.000000, 2.000000, 2.000000};
static const float Node28[3] = {-1.000000, -1.000000, -1.000000};

static float Node3[9];
static float Node5[9];
static float Node6[9];
static float Node10[3];
static float Node14[3];
static float Node15[3];
static float Node16[3];
static float Node19[9];
static float Node20;
static int32_t Node21;
static float Node25[3] = {10.000000, 10.000000, 10.000000};
static int32_t Node30;
static float Node31[3];

static node_t nodesTest[]; // Initialized below

static void Node3Instruction(instructionParam_t * param)
{
	// VectorContractionCode

	const uint32_t lStrides[] = {
		 3,
		 1,
	};
	const uint32_t rStrides[] = {
		 3,
		 1,
	};
	const uint32_t opStrides[] = {
		 3,
		 1,
	};

	for(int opIndex = 0; opIndex < sizeof(Node3) / sizeof(Node3[0]); opIndex++)
	{
		const uint32_t opIndexTuple[] = {opIndex / opStrides[0], (opIndex % opStrides[0]) / opStrides[1]};
		float sum = 0;
		for(int dim0 = 0; dim0 < 3; dim0++)
		{
			const uint32_t lIndexTuple[] = {opIndexTuple[0], dim0};
			const uint32_t rIndexTuple[] = {dim0, opIndexTuple[1]};
			sum += Node1[lIndexTuple[0] * lStrides[0] + lIndexTuple[1] * lStrides[1]] * Node2[rIndexTuple[0] * rStrides[0] + rIndexTuple[1] * rStrides[1]];
		}
		
		Node3[opIndex] = sum;
	}
}

static void Node4Instruction(instructionParam_t * param)
{
	// OutputCode

	DacOutputCallbackmatrixProd(Node3, sizeof(Node3));

	
}

static void Node6Instruction(instructionParam_t * param)
{
	// VectorContractionCode

	// VectorContractionKroneckerDeltaCode

	const uint32_t argStrides[] = {
		 3,
		 1,
	};
	const uint32_t opStrides[] = {
		 3,
		 1,
	};

	const uint32_t deltaPairs[] = {1, 0};
	for(int opIndex = 0; opIndex < sizeof(Node6) / sizeof(Node6[0]); opIndex++)
	{
		const uint32_t opIndexTuple[] = {opIndex / opStrides[0], (opIndex % opStrides[0]) / opStrides[1]};
		float sum = 0;
		for(int dim0 = 0; dim0 < 3; dim0++)
		{
			const uint32_t argIndexTuple[] = {opIndexTuple[0], dim0};
			const uint32_t kronIndexTuple[] = {dim0, opIndexTuple[1]};
			sum += Node1[argIndexTuple[0] * argStrides[0] + argIndexTuple[1] * argStrides[1]] * (kronIndexTuple[0] == kronIndexTuple[deltaPairs[0]]) * 2.000000;
		}
		
		Node6[opIndex] = sum;
	}
}

static void Node7Instruction(instructionParam_t * param)
{
	// OutputCode

	DacOutputCallbackmatrixKronProd(Node6, sizeof(Node6));

	
}

static void Node10Instruction(instructionParam_t * param)
{
	// VectorAdditionCode

	for(uint32_t dim = 0; dim < 3; dim++)
	{
		Node10[dim] = Node8[dim] + Node9[dim];
	}

}

static void Node14Instruction(instructionParam_t * param)
{
	// VectorAdditionCode

	for(uint32_t dim = 0; dim < 3; dim++)
	{
		Node14[dim] = Node12[dim] + Node13[dim];
	}

}

static void Node15Instruction(instructionParam_t * param)
{
	// VectorScalarMultiplicationCode

	for(uint32_t dim = 0; dim < 3; dim++)
	{
		Node15[dim] = Node10[dim] * Node11;
	}

}

static void Node16Instruction(instructionParam_t * param)
{
	// VectorAdditionCode

	for(uint32_t dim = 0; dim < 3; dim++)
	{
		Node16[dim] = Node14[dim] + Node15[dim];
	}

}

static void Node20Instruction(instructionParam_t * param)
{
	// VectorContractionCode

	const uint32_t lStrides[] = {
		 1,
	};
	const uint32_t rStrides[] = {
		 1,
	};
	const uint32_t opStrides[] = {
		 1,
	};

	float sum = 0;
	for(int dim0 = 0; dim0 < 3; dim0++)
	{
		const uint32_t lIndexTuple[] = {dim0};
		const uint32_t rIndexTuple[] = {dim0};
		sum += Node14[lIndexTuple[0] * lStrides[0]] * Node15[rIndexTuple[0] * rStrides[0]];
	}
	
	Node20 = sum;
}

static void Node21Instruction(instructionParam_t * param)
{
	// VectorComparisonIsSmallerCode

	float Node16Norm1 = 0;
	float Node15Norm1 = 0;
	for(uint32_t dim = 0; dim < 3; dim++)
	{
		 Node16Norm1 += Node16[dim] * Node16[dim];
	}

	for(uint32_t dim = 0; dim < 3; dim++)
	{
		 Node15Norm1 += Node15[dim] * Node15[dim];
	}

	if(Node16Norm1 < Node15Norm1)
	{
		 Node21 = 1;
	}
	else
	{
		 Node21 = 0;
	}

}

static void Node22Instruction(instructionParam_t * param)
{
	// OutputCode

	DacOutputCallbackProduct(Node15, sizeof(Node15));

	
}

static void Node23Instruction(instructionParam_t * param)
{
	// OutputCode

	DacOutputCallbackSum(Node16, sizeof(Node16));

	
}

static void Node24Instruction(instructionParam_t * param)
{
	// OutputCode

	DacOutputCallbackSmallerThan(&Node21, sizeof(Node21));

	
}

static void Node29Instruction(instructionParam_t * param)
{
	// VectorAdditionCode

	for(uint32_t dim = 0; dim < 3; dim++)
	{
		Node25[dim] = Node25[dim] + Node28[dim];
	}

}

static void Node30Instruction(instructionParam_t * param)
{
	// VectorComparisonIsSmallerCode

	float Node25Norm1 = 0;
	float Node26Norm1 = 0;
	for(uint32_t dim = 0; dim < 3; dim++)
	{
		 Node25Norm1 += Node25[dim] * Node25[dim];
	}

	for(uint32_t dim = 0; dim < 3; dim++)
	{
		 Node26Norm1 += Node26[dim] * Node26[dim];
	}

	if(Node25Norm1 < Node26Norm1)
	{
		 Node30 = 1;
	}
	else
	{
		 Node30 = 0;
	}

}

static void Node31Instruction(instructionParam_t * param)
{
	// VectorAdditionCode

	for(uint32_t dim = 0; dim < 3; dim++)
	{
		Node31[dim] = Node26[dim] + Node27[dim];
	}

}

static void Node32Instruction(instructionParam_t * param)
{
	// ControlTransferWhileCode

	if(Node30)
	{
		param->addPossiblyDeferredNode(param->instance, &nodesTest[15]);
	}
	else
	{
		param->addPossiblyDeferredNode(param->instance, &nodesTest[13]);
	}
}

static void Node33Instruction(instructionParam_t * param)
{
	// OutputCode

	DacOutputCallbackWhile(Node25, sizeof(Node25));

	
}

static node_t nodesTest[] = {
	{Node3Instruction, {}, {&nodesTest[1]}, 0, 0, 1, 3},
	{Node4Instruction, {&nodesTest[0]}, {}, 0, 1, 0, 4},
	{Node6Instruction, {}, {&nodesTest[3]}, 0, 0, 1, 6},
	{Node7Instruction, {&nodesTest[2]}, {}, 0, 1, 0, 7},
	{Node10Instruction, {}, {&nodesTest[6]}, 0, 0, 1, 10},
	{Node14Instruction, {}, {&nodesTest[7], &nodesTest[8]}, 0, 0, 2, 14},
	{Node15Instruction, {&nodesTest[4]}, {&nodesTest[7], &nodesTest[8], &nodesTest[9], &nodesTest[10]}, 0, 1, 4, 15},
	{Node16Instruction, {&nodesTest[5], &nodesTest[6]}, {&nodesTest[9], &nodesTest[11]}, 0, 2, 2, 16},
	{Node20Instruction, {&nodesTest[5], &nodesTest[6]}, {}, 0, 2, 0, 20},
	{Node21Instruction, {&nodesTest[7], &nodesTest[6]}, {&nodesTest[12]}, 0, 2, 1, 21},
	{Node22Instruction, {&nodesTest[6]}, {}, 0, 1, 0, 22},
	{Node23Instruction, {&nodesTest[7]}, {}, 0, 1, 0, 23},
	{Node24Instruction, {&nodesTest[9]}, {}, 0, 1, 0, 24},
	{Node29Instruction, {}, {&nodesTest[14], &nodesTest[17]}, 0, 0, 2, 29},
	{Node30Instruction, {&nodesTest[13]}, {&nodesTest[16]}, 0, 1, 1, 30},
	{Node31Instruction, {}, {}, 0, 0, 0, 31},
	{Node32Instruction, {&nodesTest[14]}, {}, 0, 1, 0, 32},
	{Node33Instruction, {&nodesTest[13]}, {}, 0, 1, 0, 33},
};

node_t * jobPoolInitNodesTest[] = {&nodesTest[0], &nodesTest[2], &nodesTest[4], &nodesTest[5], &nodesTest[13]};

jobPoolInit_t jobPoolInitTest = {
	.Nodes = jobPoolInitNodesTest,
	.NodesNrOf = 5,
};