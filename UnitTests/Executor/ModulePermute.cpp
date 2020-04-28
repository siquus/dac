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

#include "DacModulePermute.h"

#include "ModulePermute.h"

static ModulePermute * ModulePermutePt = nullptr;

static void projVector(const float * data, size_t size)
{
	if(NULL == ModulePermutePt)
	{
		fatal("Nullpointer!");
	}

	ModulePermutePt->ProjVector(data, size);
}

static void dProjVector(const float * data, size_t size)
{
	if(NULL == ModulePermutePt)
	{
		fatal("Nullpointer!");
	}

	ModulePermutePt->DProjVector(data, size);
}

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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 9);
	}

	called_[CALLED_TensorPermute] = true;
}

void ModulePermute::ProjVector(const float * data, size_t size)
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

	called_[CALLED_ProjVector] = true;
}

void ModulePermute::DProjVector(const float * data, size_t size)
{
	const float expected[] = {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_DProjVector] = true;
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
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
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 3);
	}

	called_[CALLED_DMatrixTransposeContracted] = true;
}

ModulePermute::ModulePermute() {
	ModulePermutePt = this;

	DacModulePermuteOutputCallbackmatrixTranspose_Register(&matrixTranspose);
	DacModulePermuteOutputCallbacktensorPermute_Register(&tensorPermute);
	DacModulePermuteOutputCallbackdMatrixTransposeContracted_Register(&dMatrixTransposeContracted);
	DacModulePermuteOutputCallbackprojVector_Register(&projVector);
	DacModulePermuteOutputCallbackdProjVector_Register(&dProjVector);
}

void ModulePermute::Execute(size_t threadsNrOf)
{
	ThreadsNrOf_ = threadsNrOf;

	DacModulePermuteRun(ThreadsNrOf_);

	for(size_t call = 0; call < sizeof(called_) / sizeof(called_[0]); call++)
	{
		if(false == called_[call])
		{
			Error("Not all callbacks executed: Missing %lu!\n", call);
		}
	}
}



