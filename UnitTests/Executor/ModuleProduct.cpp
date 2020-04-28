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

static void scalarSquared(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->ScalarSquared(data, size);
}

static void dScalarSquaredBase(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->DScalarSquaredBase(data, size);
}

static void vectorSquared(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->VectorSquared(data, size);
}

static void dvectorSquaredBase(const float * data, size_t size)
{
	if(NULL == ModuleProductPt)
	{
		fatal("Nullpointer!");
	}

	ModuleProductPt->DvectorSquaredBase(data, size);
}

void ModuleProduct::VectorSquared(const float * data, size_t size)
{
	const float expected[] = {1, 4, 9};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_VectorSquared] = true;
}

void ModuleProduct::DvectorSquaredBase(const float * data, size_t size)
{
	const float expected[] = {
			2, 0, 0,
			0, 4, 0,
			0, 0, 6};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_DvectorSquaredBase] = true;
}

void ModuleProduct::ScalarSquared(const float * data, size_t size)
{
	const float expected[] = {1764};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! %f\n", (double) data[0]);
	}

	called_[CALLED_ScalarSquared] = true;
}

void ModuleProduct::DScalarSquaredBase(const float * data, size_t size)
{
	const float expected[] = {84};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result! %f\n", (double) data[0]);
	}

	called_[CALLED_DScalarSquaredBase] = true;
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
		Error("Unexpected result! %f\n", (double) data[0]);
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
		Error("Unexpected result! %f\n", (double) data[0]);
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
		Error("Unexpected result! %f\n", (double) data[0]);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
	DacModuleProductOutputCallbackscalarSquared_Register(&scalarSquared);
	DacModuleProductOutputCallbackdScalarSquaredBase_Register(&dScalarSquaredBase);
	DacModuleProductOutputCallbackvectorSquared_Register(&vectorSquared);
	DacModuleProductOutputCallbackdvectorSquaredBase_Register(&dvectorSquaredBase);
}

void ModuleProduct::Execute(size_t threadsNrOf)
{
	ThreadsNrOf_ = threadsNrOf;

	DacModuleProductRun(ThreadsNrOf_);

	for(size_t call = 0; call < sizeof(called_) / sizeof(called_[0]); call++)
	{
		if(false == called_[call])
		{
			Error("Not all callbacks executed: Missing %lu!\n", call);
		}
	}
}
