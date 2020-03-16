/*
 * ModuleProduct.cpp
 *
 *  Created on: Mar 16, 2020
 *      Author: derommo
 */

#include <string.h>

#include "error_functions.h"

#include "DacModuleProduct.h"

#include "ModuleProduct.h"

static ModuleProduct * ModuleProductPt = nullptr;

static void vecVecProd(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->VecVecProduct(data, size);
}

void ModuleProduct::VecVecProduct(const float * data, size_t size)
{
	const float expected[] = {
			4, 5, 6,
			8, 10, 12,
			12, 15, 18,
	};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f, %f, %f, %f, %f, %f, %f\n",
				data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
	}

	called_[CALLED_VecVecProduct] = true;
}

ModuleProduct::ModuleProduct() {
	ModuleProductPt = this;

	DacModuleProductOutputCallbackvecVecProd_Register(&vecVecProd);
}

void ModuleProduct::Execute(size_t threadsNrOf)
{
	ThreadsNrOf_ = threadsNrOf;

	DacModuleProductRun(ThreadsNrOf_);

	for(size_t call = 0; call < sizeof(called_) / sizeof(called_[0]); call++)
	{
		if(false == called_[call])
		{
			Error("Not all callbacks executed!\n");
			break;
		}
	}
}
