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

static void vecMatrixProd(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->VecMatrixProd(data, size);
}

static void tensorVecContr2(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->TensorVecContr2(data, size);
}

static void tensorVecContr1(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->TensorVecContr1(data, size);
}

static void tensorMatrixContr1(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->TensorMatrixContr1(data, size);
}

static void tensorMatrixContr12(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->TensorMatrixContr12(data, size);
}

static void matrixIdProd(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->MatrixIdProd(data, size);
}

static void twoMatrixTrace(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->TwoMatrixTrace(data, size);
}

static void matrixProdLeft(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->MatrixProdLeft(data, size);
}

static void matrixProdRight(const float * data, size_t size)
{
	if(NULL == ModuleContractPt)
	{
		fatal("Nullpointer!");
	}

	ModuleContractPt->MatrixProdRight(data, size);
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
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_MatrixProd1] = true;
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_MatrixVecProd] = true;
}

void ModuleContract::VecMatrixProd(const float * data, size_t size)
{
	const float expected[] = {14, 32, 50};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_VecMatrixProd] = true;
}

void ModuleContract::TensorVecContr2(const float * data, size_t size)
{
	const float expected[] = {
			14, 32, 50,
			68, 86, 109,
			128, 146, 164};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_TensorVecContr2] = true;
}

void ModuleContract::TensorVecContr1(const float * data, size_t size)
{
	const float expected[] = {
			30, 36, 42,
			84, 93, 99,
			144, 150, 156};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_TensorVecContr1] = true;
}

void ModuleContract::TensorMatrixContr1(const float * data, size_t size)
{
	const float expected[] = {
			66., 78., 90., 78., 93., 108., 90., 108., 126.,
			174., 213., 252., 193., 236., 279., 205., 251., 297.,
			294., 363., 432., 306., 378., 450., 318., 393., 468.};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 9);
	}

	called_[CALLED_TensorMatrixContr1] = true;
}

void ModuleContract::TensorMatrixContr12(const float * data, size_t size)
{
	const float expected[] = {285, 707, 1140};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_TensorMatrixContr12] = true;
}

void ModuleContract::MatrixIdProd(const float * data, size_t size)
{
	const float expected[] = {2, 4, 6, 8, 10, 12, 14, 16, 18};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_MatrixIdProd] = true;
}

void ModuleContract::TwoMatrixTrace(const float * data, size_t size)
{
	const float expected = 30.;

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(*data != expected)
	{
		Error("Unexpected result! %f", (double) data[0]);
	}

	called_[CALLED_TwoMatrixTrace] = true;
}

void ModuleContract::MatrixProdLeft(const float * data, size_t size)
{
	const float expected[] = {
			3, 0, 0,
			0, 3, 0,
			0, 0, 3,
	};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_MatrixProdLeft] = true;
}

void ModuleContract::MatrixProdRight(const float * data, size_t size)
{
	const float expected[] = {
			3, 12, 21,
			6, 15, 24,
			9, 18, 27,
	};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_MatrixProdRight] = true;
}

ModuleContract::ModuleContract() {
	ModuleContractPt = this;

	DacModuleContractOutputCallbackmatrixVecProd_Register(&matrixVecProd);
	DacModuleContractOutputCallbackvecMatrixProd_Register(&vecMatrixProd);
	DacModuleContractOutputCallbackmatrixProd1_Register(&matrixProd1);
	DacModuleContractOutputCallbacktensorVecContr2_Register(&tensorVecContr2);
	DacModuleContractOutputCallbacktensorVecContr1_Register(&tensorVecContr1);
	DacModuleContractOutputCallbacktensorMatrixContr1_Register(&tensorMatrixContr1);
	DacModuleContractOutputCallbacktensorMatrixContr12_Register(&tensorMatrixContr12);
	DacModuleContractOutputCallbackmatrixIdProd_Register(&matrixIdProd);
	DacModuleContractOutputCallbacktwoMatrixTrace_Register(&twoMatrixTrace);
	DacModuleContractOutputCallbackmatrixProdRight_Register(&matrixProdRight);
	DacModuleContractOutputCallbackmatrixProdLeft_Register(&matrixProdLeft);
}

void ModuleContract::Execute(size_t threadsNrOf)
{
	ThreadsNrOf_ = threadsNrOf;

	DacModuleContractRun(ThreadsNrOf_);

	for(size_t call = 0; call < sizeof(called_) / sizeof(called_[0]); call++)
	{
		if(false == called_[call])
		{
			Error("Not all callbacks executed!\n");
			break;
		}
	}
}

