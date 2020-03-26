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

static void scalarScalarDiv(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->ScalarScalarDiv(data, size);
}

static void dScalarScalarDivLeft(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->DScalarScalarDivLeft(data, size);
}

static void dScalarScalarDivRight(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->DScalarScalarDivRight(data, size);
}

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

static void dVecScalarProdLeft(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->DVecScalarProdLeft(data, size);
}

static void dVecScalarProdRight(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->DVecScalarProdRight(data, size);
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

	ModuleProductPt->DVecVecProductLeft(data, size);
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

	ModuleProductPt->KronVecProduct(data, size);
}

static void scalar2Squared(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->Scalar2Squared(data, size);
}

static void dScalar2SquaredBase(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->DScalar2SquaredBase(data, size);
}

void ModuleProduct::Scalar2Squared(const float * data, size_t size)
{
	const float expected[] = {4};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! %f\n", data[0]);
	}

	called_[CALLED_Scalar2Squared] = true;
}

void ModuleProduct::DScalar2SquaredBase(const float * data, size_t size)
{
	const float expected[] = {4};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! %f\n", data[0]);
	}

	called_[CALLED_DScalar2SquaredBase] = true;
}

void ModuleProduct::ScalarScalarDiv(const float * data, size_t size)
{
	const float expected[] = {21};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! %f\n", data[0]);
	}

	called_[CALLED_ScalarScalarDiv] = true;
}

void ModuleProduct::DScalarScalarDivLeft(const float * data, size_t size)
{
	const float expected[] = {1./2.};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! %f\n", data[0]);
	}

	called_[CALLED_DScalarScalarDivLeft] = true;
}

void ModuleProduct::DScalarScalarDivRight(const float * data, size_t size)
{
	const float expected[] = {-42. / 4.};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! %f\n", data[0]);
	}

	called_[CALLED_DScalarScalarDivRight] = true;
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
	const float expected[] = {12, 24, 36};

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
	const float expected[] = {12, 24, 36};

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

void ModuleProduct::DVecScalarProdLeft(const float * data, size_t size)
{
	const float expected[] = {
			42, 0, 0,
			0, 42, 0,
			0, 0, 42};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f, %f, %f, %f, %f, %f, %f)\n",
				data[0], data[1], data[2],
				data[3], data[4], data[5],
				data[6], data[7], data[8]);
	}

	called_[CALLED_DVecScalarProdLeft] = true;
}

void ModuleProduct::DVecScalarProdRight(const float * data, size_t size)
{
	const float expected[] = {1, 2, 3};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! (%f, %f, %f)\n",
				data[0], data[1], data[2]);
	}

	called_[CALLED_DVecScalarProdRight] = true;
}

void ModuleProduct::DVecVecProductLeft(const float * data, size_t size)
{
	const float expected[] = {24, 30, 36};

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
	const float expected[] = {6, 12, 18};

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

	DacModuleProductOutputCallbackscalarScalarDiv_Register(&scalarScalarDiv);
	DacModuleProductOutputCallbackdScalarScalarDivLeft_Register(&dScalarScalarDivLeft);
	DacModuleProductOutputCallbackdScalarScalarDivRight_Register(&dScalarScalarDivRight);
	DacModuleProductOutputCallbackscalarVecProd_Register(&scalarVecProd);
	DacModuleProductOutputCallbackvecScalarProd_Register(&vecScalarProd);
	DacModuleProductOutputCallbackvecVecProd_Register(&vecVecProd);
	DacModuleProductOutputCallbackvecKronProd_Register(&vecKronProd);
	DacModuleProductOutputCallbackkronVecProd_Register(&kronVecProd);
	DacModuleProductOutputCallbackdVecScalarProdLeft_Register(&dVecScalarProdLeft);
	DacModuleProductOutputCallbackdVecScalarProdRight_Register(&dVecScalarProdRight);
	DacModuleProductOutputCallbackdVecVecProdLeft_Register(&dVecVecProdLeft);
	DacModuleProductOutputCallbackdVecVecProdRight_Register(&dVecVecProdRight);
	DacModuleProductOutputCallbackscalar2Squared_Register(&scalar2Squared);
	DacModuleProductOutputCallbackdScalar2SquaredBase_Register(&dScalar2SquaredBase);
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
