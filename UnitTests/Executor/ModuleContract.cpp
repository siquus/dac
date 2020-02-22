/*
 * ModuleContract.cpp
 *
 *  Created on: Feb 16, 2020
 *      Author: derommo
 */

#include <string.h>

#include "error_functions.h"

#include "DacModuleContract.h"

#include "ModuleContract.h"

static ModuleContract * ModuleContractPt = nullptr;


static void matrixProd1(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->MatrixProd1(data, size);
}

static void matrixVecProd(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->MatrixVecProd(data, size);
}

void ModuleContract::MatrixProd1(const float * data, size_t size)
{
	const float expected[] = {
			1, 2, 3,
			4, 5, 6,
			7, 8, 9,
	};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
	}

	MatrixProd1Called_ = true;
}

void ModuleContract::MatrixVecProd(const float * data, size_t size)
{
	const float expected[] = {14, 32, 50};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result (%f, %f, %f)\n", data[0], data[1], data[2]);
	}

	MatrixVecProdCalled_ = true;
}

ModuleContract::ModuleContract() {
	ModuleContractPt = this;

	DacModuleContractOutputCallbackmatrixVecProd_Register(&matrixVecProd);
	DacModuleContractOutputCallbackmatrixProd1_Register(&matrixProd1);
}

void ModuleContract::Execute(size_t threadsNrOf)
{
	ThreadsNrOf_ = threadsNrOf;

	DacModuleContractRun(ThreadsNrOf_);

	if(!MatrixProd1Called_ || !MatrixVecProdCalled_)
	{
		Error("Not all callbacks executed!\n");
	}
}

