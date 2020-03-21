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

static void vecScalarProd(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->VecScalarProduct(data, size);
}

static void scalarVecProd(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->ScalarVecProduct(data, size);
}

static void vecVecProd(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->VecVecProduct(data, size);
}

static void dVecVecProdRight(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->DVecVecProductRight(data, size);
}

static void dVecVecProdLeft(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->KronVecProduct(data, size);
}

static void vecKronProd(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->VecKronProduct(data, size);
}

static void kronVecProd(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->DVecVecProductLeft(data, size);
}

void ModuleProduct::ScalarVecProduct(const float * data, size_t size)
{
	const float expected[] = {42, 84, 126};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f)\n",
				data[0], data[1], data[2]);
	}

	called_[CALLED_ScalarVecProduct] = true;
}

void ModuleProduct::VecScalarProduct(const float * data, size_t size)
{
	const float expected[] = {42, 84, 126};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f)\n",
				data[0], data[1], data[2]);
	}

	called_[CALLED_VecScalarProduct] = true;
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
		Error("Unexpected result! (%f, %f, %f, %f, %f, %f, %f, %f, %f)\n",
				data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
	}

	called_[CALLED_VecVecProduct] = true;
}

void ModuleProduct::KronVecProduct(const float * data, size_t size)
{
	const float expected[] = {4, 5, 6};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f)\n",
				data[0], data[1], data[2]);
	}

	called_[CALLED_KronVecProduct] = true;
}

void ModuleProduct::VecKronProduct(const float * data, size_t size)
{
	const float expected[] = {4, 5, 6};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f)\n",
				data[0], data[1], data[2]);
	}

	called_[CALLED_VecKronProduct] = true;
}



void ModuleProduct::DVecVecProductLeft(const float * data, size_t size)
{
	const float expected[] = {4, 5, 6};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f)\n",
				data[0], data[1], data[2]);
	}

	called_[CALLED_DVecVecProductLeft] = true;
}

void ModuleProduct::DVecVecProductRight(const float * data, size_t size)
{
	const float expected[] = {4, 5, 6};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f)\n",
				data[0], data[1], data[2]);
	}

	called_[CALLED_DVecVecProductRight] = true;
}

ModuleProduct::ModuleProduct() {
	ModuleProductPt = this;

	DacModuleProductOutputCallbackscalarVecProd_Register(&scalarVecProd);
	DacModuleProductOutputCallbackvecScalarProd_Register(&vecScalarProd);
	DacModuleProductOutputCallbackvecVecProd_Register(&vecVecProd);
	DacModuleProductOutputCallbackvecKronProd_Register(&vecKronProd);
	DacModuleProductOutputCallbackkronVecProd_Register(&kronVecProd);
	DacModuleProductOutputCallbackdVecVecProdLeft_Register(&dVecVecProdLeft);
	DacModuleProductOutputCallbackdVecVecProdRight_Register(&dVecVecProdRight);
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
