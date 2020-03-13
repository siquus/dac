/*
 * ModulePermute.cpp
 *
 *  Created on: Feb 29, 2020
 *      Author: derommo
 */

#include <string.h>

#include "error_functions.h"

#include "DacModulePermute.h"

#include "ModulePermute.h"

static ModulePermute * ModulePermutePt = nullptr;

static void matrixTranspose(const float * data, size_t size)
{
	if(NULL == ModulePermutePt)
	{
		fatal("Nullpointer!");
	}

	ModulePermutePt->MatrixTranspose(data, size);
}

static void dMatrixTransposeContracted(const float * data, size_t size)
{
	if(NULL == ModulePermutePt)
	{
		fatal("Nullpointer!");
	}

	ModulePermutePt->DMatrixTransposeContracted(data, size);
}

static void tensorPermute(const float * data, size_t size)
{
	if(NULL == ModulePermutePt)
	{
		fatal("Nullpointer!");
	}

	ModulePermutePt->TensorPermute(data, size);
}

void ModulePermute::TensorPermute(const float * data, size_t size)
{
	const float expected[] = {
			1., 10., 20., 4., 13., 23., 7., 16., 26.,
			2., 11., 21., 5., 14., 24., 8., 18., 27.,
			3., 12., 22., 6., 15., 25., 9., 19., 28.};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (\n");

		for(size_t entry = 0; entry < sizeof(expected) / sizeof(expected[0]); entry++)
		{
			fprintf(stderr, "%f, ", data[entry]);

			if((0 == (entry % 9)) && entry)
			{
				fprintf(stderr, "\n");
			}
		}
		fprintf(stderr, ")\n");
	}

	called_[CALLED_TensorPermute] = true;
}

void ModulePermute::MatrixTranspose(const float * data, size_t size)
{
	const float expected[] = {1, 4, 7, 2, 5, 8, 3, 6, 9};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f, %f, %f, %f, %f, %f, %f\n",
				data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
	}

	called_[CALLED_MatrixTranspose] = true;
}

void ModulePermute::DMatrixTransposeContracted(const float * data, size_t size)
{
	const float expected[] = {1, 4, 7, 2, 5, 8, 3, 6, 9};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f, %f, %f, %f, %f, %f, %f\n",
				data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
	}

	called_[CALLED_DMatrixTransposeContracted] = true;
}

ModulePermute::ModulePermute() {
	ModulePermutePt = this;

	DacModulePermuteOutputCallbackmatrixTranspose_Register(&matrixTranspose);
	DacModulePermuteOutputCallbacktensorPermute_Register(&tensorPermute);
	DacModulePermuteOutputCallbackdMatrixTransposeContracted_Register(&dMatrixTransposeContracted);
}

void ModulePermute::Execute(size_t threadsNrOf)
{
	ThreadsNrOf_ = threadsNrOf;

	DacModulePermuteRun(ThreadsNrOf_);

	for(size_t call = 0; call < sizeof(called_) / sizeof(called_[0]); call++)
	{
		if(false == called_[call])
		{
			Error("Not all callbacks executed!\n");
			break;
		}
	}
}



