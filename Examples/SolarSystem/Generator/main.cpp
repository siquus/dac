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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <vector>

#include "common.h"

#include "ControlTransfer.h"
#include "Graph.h"
#include "Module.h"
#include "Ring.h"
#include "Interface.h"
#include "CodeGenerator.h"

#define FATAL_ON_FALSE(arg) if(!arg){fatalLine(__FILE__, __LINE__, #arg);}

typedef struct {
	int32_t Iterations = 100000;
	float Stepsize = 1.0;
} cmdLineArgs_t;

typedef enum {
	CMD_LINE_OPTION_HELP,
	CMD_LINE_OPTION_ITERATIONS_NROF,
	CMD_LINE_OPTION_STEPSIZE,
	CMD_LINE_OPTION_NROF,
} cmdLineOption_t;

typedef struct {
	char Option[5];
	char Param[4];
	char Name[100];
	char Help[100];
} cmdLineArgument_t;;

static const cmdLineArgument_t cmdLineArguments[CMD_LINE_OPTION_NROF] =
{
		{"-h", "", "Help", "Prints this help"},
		{"-i", "%i", "Iterations", "Number of simulation steps"},
		{"-s", "%f", "Stepsize", "Time increment of simulation step"}
};

static const float GravitationalConstant = 2.95912208286E-4;

static void fatalLine(const char * file, int line, const char * lineString)
{
	fprintf(stderr, "%s:%i: Code generation failed for \"%s\"!\n",
			file, line, lineString);
	fflush(stderr);
	exit(1);
}

static uint32_t Factorial(uint32_t n)
{
	if(1 >= n)
	{
		return 1;
	}

	uint32_t factorial = n;
	uint32_t factor = n - 1;
	while(factor)
	{
		factorial *= factor;
		factor--;
	}

	return factorial;
}

static uint32_t BinomialCoefficient(uint32_t n, uint32_t k)
{
	if(k > n)
	{
		return 0;
	}

	uint32_t nFactorial = Factorial(n);
	uint32_t kFactorial = Factorial(k);
	uint32_t nMkFactorial = Factorial(n - k);

	return nFactorial / (kFactorial * nMkFactorial);
}

// Creates a matrix that will generate a vector of unique differences (q_1_1 - q_2_1, q_1_2 - q_2_2, ..),
// where q_[ObjectNr]_[DimQ] :
//
// 1 0 0 -1  0  ...
// 0 1 0  0 -1  ...
// ....
// i.e. it will then be multiplied on the right by (q_1_1, q_1_2, q_1_3, q_2_1, ...)
static const Algebra::Module::VectorSpace::Vector * DifferenceGeneratorMatrix(
		Graph * graph,
		uint32_t objectsNrOf,
		uint32_t objectDimNrOf)
{
	const uint32_t matrixRows = objectDimNrOf * BinomialCoefficient(objectsNrOf, 2);
	const uint32_t matrixColumns = objectsNrOf * objectDimNrOf;

	// TODO: For many objects initialize this matrix as sparse!
	auto diffMatrixValue = new std::vector<float>(matrixRows * matrixColumns, 0);

	uint32_t currentRow = 0;
	for(uint32_t object1 = 0; object1 < objectsNrOf; object1++)
	{
		for(uint32_t object2 = object1 + 1; object2 < objectsNrOf; object2++)
		{
			for(uint8_t dim = 0; dim < objectDimNrOf; dim++)
			{
				uint32_t colObj1 = object1 * objectDimNrOf + dim;
				diffMatrixValue->at(currentRow * matrixColumns + colObj1) = 1;

				uint32_t colObj2 = object2 * objectDimNrOf + dim;
				diffMatrixValue->at(currentRow * matrixColumns + colObj2) = -1;

				currentRow++;
			}
		}
	}

	auto diffSpace = new Algebra::Module::VectorSpace(
			Algebra::Ring::Float32,
			std::vector<dimension_t>{matrixRows, matrixColumns});

	Algebra::Module::VectorSpace::Vector::propertyParameterSparse_t paramSparse;
	paramSparse.Initializer = paramSparse.DENSE;

	return diffSpace->Element(graph,
			*diffMatrixValue,
			Algebra::Module::VectorSpace::Vector::Property::Sparse,
			&paramSparse);
}

// Creates a matrix that sums every sumDimensions vector elements, e.g. for sumDimensions = 3
//
// 1 1 1 0 0 0 0 0 0 0 0 ...
// 0 0 0 1 1 1 0 0 0 0 0 ...
// ...
//
static const Algebra::Module::VectorSpace::Vector * PartialVectorSumMatrix(
		Graph * graph,
		uint32_t vectorSize,
		uint32_t sumDimensions)
{
	if(vectorSize % sumDimensions)
	{
		return nullptr;
	}

	const uint32_t rows =  vectorSize / sumDimensions;
	const uint32_t columns = vectorSize;

	// TODO: For many dimensions initialize this matrix as sparse!
	auto sumMatrixValue = new std::vector<float>(rows * columns, 0);

	for(uint32_t row = 0; row < rows; row++)
	{
		for(uint32_t sum = 0; sum < sumDimensions; sum++)
		{
			uint32_t column = sumDimensions * row + sum;
			sumMatrixValue->at(row * columns + column) = 1;
		}
	}

	auto sumSpace = new Algebra::Module::VectorSpace(
			Algebra::Ring::Float32,
			std::vector<dimension_t>{rows, columns});

	Algebra::Module::VectorSpace::Vector::propertyParameterSparse_t paramSparse;
	paramSparse.Initializer = paramSparse.DENSE;

	return sumSpace->Element(graph,
			*sumMatrixValue,
			Algebra::Module::VectorSpace::Vector::Property::Sparse,
			&paramSparse);
}

// Creates the vector that multiplies with the vector ( 1 / |q_1 - q_2|, 1 / |q_1 - q_3|, ...)
// to form \Sum_{i<j} m_i*m_j / |q_i - q_j|
// i.e.
// (m_1 * m_2, m_1 * m_3, ...)
static const Algebra::Module::VectorSpace::Vector * MassWeightedSumVector(
		Graph * graph,
		const objectData_t * objects,
		size_t objectsNrOf)
{
	const uint32_t vectorLength = BinomialCoefficient(objectsNrOf, 2);
	auto vectorValue = new std::vector<float>(vectorLength);

	size_t index = 0;
	for(size_t object1 = 0; object1 < objectsNrOf; object1++)
	{
		for(size_t object2 = object1 + 1; object2 < objectsNrOf; object2++)
		{
			vectorValue->at(index) = objects[object1].Mass * objects[object2].Mass;
			index++;
		}
	}

	auto space = new Algebra::Module::VectorSpace(
				Algebra::Ring::Float32,
				vectorLength);

	return space->Element(graph, *vectorValue);
}

// Creates the Symplectic Matrix for the standard coordinates
// (q1, q2, ..., qn, p1, p2, ..., pn). It has the form:
//
//      0 id
//    -id  0
//
static const Algebra::Module::VectorSpace::Vector * SymplecticMatrix(
		Graph * graph,
		uint32_t dimensions)
{
	if(dimensions % 2)
	{
		return nullptr;
	}

	// TODO: For many dimensions initialize this matrix as sparse!
	auto matrixValue = new std::vector<float>(dimensions * dimensions, 0);

	// TODO: Really inefficient way of setting the non-zero values
	for(size_t index = 0; index < matrixValue->size(); index++)
	{
		size_t row = index / dimensions;
		size_t column = index % dimensions;

		if((column == (row + dimensions / 2)) && (row < dimensions / 2))
		{
			matrixValue->at(index) = 1;
		}
		else if(column == (row - dimensions / 2))
		{
			matrixValue->at(index) = -1;
		}
	}

	std::map<Algebra::Module::VectorSpace::Vector::Property, const void *> properties;

	// Set sparse property
	auto paramSparse = new Algebra::Module::VectorSpace::Vector::propertyParameterSparse_t;
	paramSparse->Initializer = paramSparse->DENSE;

	properties.insert({
		Algebra::Module::VectorSpace::Vector::Property::Sparse,
		paramSparse
	});

	// Set antisymmetric
	properties.insert({
			Algebra::Module::VectorSpace::Vector::Property::Antisymmetric,
			nullptr
		});

	auto space = new Algebra::Module::VectorSpace(
			Algebra::Ring::Float32,
			std::vector<dimension_t>{dimensions, dimensions});

	return space->Element(
			graph,
			*matrixValue,
			properties);
}

static void printHelp()
{
	printf("\n");
	for(int option = 0; option < CMD_LINE_OPTION_NROF; option++)
	{
		printf("%s\t %s\t %s: %s\n",
				cmdLineArguments[option].Option,
				cmdLineArguments[option].Param,
				cmdLineArguments[option].Name,
				cmdLineArguments[option].Help);
	}
	printf("\n");
}

static void handleCmdLineOption(cmdLineArgs_t * cmdLineArgs, cmdLineOption_t option, const char* arg)
{
	switch(option)
	{
	case CMD_LINE_OPTION_STEPSIZE:
	{
		errno = 0;
		char * tailptr;
		cmdLineArgs->Stepsize = strtof(arg, &tailptr);
		if(errno)
		{
			Fatal("Could not convert \"%s\" to Number: %s!\n",
					arg,
					strerror(errno));
		}
		else if(arg == tailptr)
		{
			Fatal("Could not convert \"%s\" to Number!\n", arg);
		}
		else if(0.f >= cmdLineArgs->Stepsize)
		{
			Fatal("%f is not a valid number for stepsize!\n", (double) cmdLineArgs->Stepsize);
		}
	}
		break;

	case CMD_LINE_OPTION_ITERATIONS_NROF:
	{
		errno = 0;
		char * tailptr;
		cmdLineArgs->Iterations = strtol(arg, &tailptr, 10);

		if(errno)
		{
			Fatal("Could not convert \"%s\" to Number: %s!\n",
					arg,
					strerror(errno));
		}
		else if(arg == tailptr)
		{
			Fatal("Could not convert \"%s\" to Number!\n", arg);
		}
		else if(0 >= cmdLineArgs->Iterations)
		{
			Fatal("%i is not a valid number of iterations!\n", cmdLineArgs->Iterations);
		}
	}
		break;

	default: // no break intended
	case CMD_LINE_OPTION_NROF:
		Fatal("Unhandled option nr %u!\n", option);
	}
}

static void parseCmdLineArgs(cmdLineArgs_t * cmdLineArgs, int argc, char* argv[])
{
	for(int arg = 1; arg < argc; arg++)
	{
		bool foundOption = false;
		for(int option = 0; option < CMD_LINE_OPTION_NROF; option++)
		{
			if(0 == strncmp(cmdLineArguments[option].Option, argv[arg], sizeof(cmdLineArguments[option])))
			{
				foundOption = true;

				if(CMD_LINE_OPTION_HELP == option)
				{
					printHelp();
					exit(0);
				}

				if(arg + 1 >= argc)
				{
					printHelp();
					Fatal("Missing parameter for %s\n", cmdLineArguments[option].Option);
				}

				arg++;

				handleCmdLineOption(cmdLineArgs, (cmdLineOption_t) option, argv[arg]);

				break;
			}
		}

		if(!foundOption)
		{
			printHelp();
			Fatal("Unknown Option: %s\n", argv[arg]);
		}
	}
}


int main(int argc, char* argv[])
{
	cmdLineArgs_t cmdLineArgs;
	parseCmdLineArgs(&cmdLineArgs, argc, argv);

	Graph graph("SolarSystem");

	// Create initial state
	// = (                        Q                         ,                P          )
	// = (Object1PosX, Object1PosY, Object1PosZ, Object2PosX, ..., Object1MomentumX, ...)
	std::vector<float> initialStateData(2 * DIMENSIONS * OBJECT_NROF);
	for(int object = 0; object < OBJECT_NROF; object++)
	{
		for(uint8_t dim = 0; dim < DIMENSIONS; dim++)
		{
			initialStateData[DIMENSIONS * object + dim] = Objects[object].InitialPosition[dim];
			initialStateData[DIMENSIONS * OBJECT_NROF + DIMENSIONS * object + dim] = Objects[object].InitialVelocity[dim] * Objects[object].Mass;
		}
	}

	auto phaseSpace = Algebra::Module::VectorSpace(Algebra::Ring::Float32, 2 * DIMENSIONS * OBJECT_NROF);

	auto state = phaseSpace.Element(&graph, initialStateData);

	// Generate the Hamiltonian
	// H =           T         -                   V
	//   = \Sum_i p_i^2 / 2m_i - G * \Sum_{i<j} m_i*m_j / |q_i - q_j|


	// Start with the kinetic energy T
	auto momentumState = state->Project(std::pair<uint32_t, uint32_t>{DIMENSIONS * OBJECT_NROF, 2 * DIMENSIONS * OBJECT_NROF});

	std::vector<float> diagMasses(DIMENSIONS * OBJECT_NROF);
	for(size_t object = 0; object < OBJECT_NROF; object++)
	{
		float value = 1.f / (2.f * Objects[object].Mass);

		for(uint8_t dim = 0; dim < DIMENSIONS; dim++)
		{
			diagMasses[DIMENSIONS * object + dim] = value;
		}
	}

	auto massDiag = momentumState->Space()->Homomorphism(
			&graph,
			diagMasses,
			Algebra::Module::VectorSpace::Vector::Property::Diagonal);

	auto momentumDivMass = momentumState->Contract(massDiag, 0, 0);
	auto kineticEnergy = momentumDivMass->Contract(momentumState);

	// Continue with the potential energy V
	auto diffGenMatrix = DifferenceGeneratorMatrix(&graph, OBJECT_NROF, DIMENSIONS);

	auto positionState = state->Project(std::pair<uint32_t, uint32_t>{0, DIMENSIONS * OBJECT_NROF});

	// qDiffs = (q1_1 - q2_1, q1_2 - q2_2, q1_3 - q2_3, ...)
	auto qDiffs = diffGenMatrix->Contract(positionState, 1, 0);

	// We need Sum_{i < j} 1 / |qi - qj|, ... So let's create a vector (|q1 - q2|, ...)
	// Create ((q1_1 - q2_1)^2, (q1_2 - q2_2)^2, (q1_3 - q2_3)^2, ..)
	auto qDiffsSquared = qDiffs->Power(2.f);

	// Create ((q1_1 - q2_1)^2 + (q1_2 - q2_2)^2 + (q1_3 - q2_3)^2, ..)
	auto partialSumMatrix = PartialVectorSumMatrix(
			&graph,
			qDiffsSquared->Space()->GetDim(),
			DIMENSIONS);

	auto qDiffsSquaredSummed = partialSumMatrix->Contract(qDiffsSquared, 1, 0);

	// Create (Sqrt((q1_1 - q2_1)^2 + (q1_2 - q2_2)^2 + (q1_3 - q2_3)^2), ..) = (|q1 - q2|, ...)
	auto qDiffsNorm = qDiffsSquaredSummed->Power(1.f / 2.f);

	// Create (1 / |q1 - q2|, ...)
	auto qDiffsNormInv = qDiffsNorm->Power(-1.f);

	// Create V / G = \Sum_{i<j} m_i*m_j / |q_i - q_j|
	auto massVector = MassWeightedSumVector(
			&graph,
			Objects,
			sizeof(Objects) / sizeof(Objects[0]));

	auto VmissinG = massVector->Contract(qDiffsNormInv);

	auto potentialEnergy = VmissinG->Multiply(GravitationalConstant);

	auto hamiltonian = kineticEnergy->Subtract(potentialEnergy);

	// Calculate the Hamiltonian vector field X_H = J * dH, where J is the symplectic matrix
	auto symplecticMatrix = SymplecticMatrix(
			&graph,
			state->Space()->GetDim());

	auto dH = hamiltonian->Derivative(state);

	auto X_H = symplecticMatrix->Contract(dH, 1, 0);

	auto timeIncrement = X_H->Space()->Scalar(&graph, cmdLineArgs.Stepsize);
	auto step = X_H->Multiply(timeIncrement);

	auto newState = state->Add(step);
	newState->StoreIn(state);

	Interface::Output Output(&graph, "NewState");
	Output.Set(newState);

	auto iterationVs = Algebra::Module::VectorSpace(Algebra::Ring::Int32, 1);

	auto SimIterations = iterationVs.Scalar(&graph, cmdLineArgs.Iterations);
	auto minusOne = iterationVs.Scalar(&graph, -1);

	auto IterationCntDown = SimIterations->Add(minusOne);
	IterationCntDown->StoreIn(SimIterations);

	std::vector<const NodeRef *> whileParents{&Output};

	ControlTransfer::While While;
	While.Set(
			IterationCntDown,
			whileParents,
			&Output,
			nullptr);

	// Perform graph optimization
	graph.RemoveDuplicates();

	// Generate Code

	struct stat stCodePath = {};
	const char path[] = "../Executor/dac";
	// Test if folder already exsists
	if (stat(path, &stCodePath) == -1)
	{
		mkdir(path, 0700);
	}

	auto outpath = std::string(path) + "/";

	CodeGenerator codeGenerator(&outpath);
	FATAL_ON_FALSE(codeGenerator.Generate(&graph));


	printf("Success!\n");
	return 0;
}


