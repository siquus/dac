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

#include "DacModuleCNN.h"

#include "ModuleCNN.h"

static const float input21[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
static const float input42[] = {0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0};

static ModuleCNN * ModuleCNNPt = nullptr;

static const float * vectorInput(size_t identifier, size_t size)
{
	if(NULL == ModuleCNNPt)
	{
		fatal("Nullpointer!");
	}

	return ModuleCNNPt->VectorInput(identifier, size);
}

static void maxInput(const float * data, size_t size)
{
	if(NULL == ModuleCNNPt)
	{
		fatal("Nullpointer!");
	}

	ModuleCNNPt->MaxInput(data, size);
}

static void cc(const float * data, size_t size)
{
	if(NULL == ModuleCNNPt)
	{
		fatal("Nullpointer!");
	}

	ModuleCNNPt->CC(data, size);
}

static void ccMaxPool(const float * data, size_t size)
{
	if(NULL == ModuleCNNPt)
	{
		fatal("Nullpointer!");
	}

	ModuleCNNPt->CCMaxPool(data, size);
}

static void vectorSplit(const float * data, size_t size)
{
	if(NULL == ModuleCNNPt)
	{
		fatal("Nullpointer!");
	}

	ModuleCNNPt->VectorSplit(data, size);
}

static void vector21(const float * data, size_t size)
{
	if(NULL == ModuleCNNPt)
	{
		fatal("Nullpointer!");
	}

	ModuleCNNPt->Vector21(data, size);
}

static void vector42(const float * data, size_t size)
{
	if(NULL == ModuleCNNPt)
	{
		fatal("Nullpointer!");
	}

	ModuleCNNPt->Vector42(data, size);
}

void ModuleCNN::Vector21(const float * data, size_t size)
{
	if(sizeof(input21) != size)
		{
			Error("Size Mismatch! %lu vs %lu\n", sizeof(input21), size);
		}
		else if(memcmp(data, input21, sizeof(input21)))
		{
			Error("Unexpected result!\n");
			PrintMatrix(stderr, data, size, 9);
		}

		called_[CALLED_Vector21] = true;
}

void ModuleCNN::MaxInput(const float * data, size_t size)
{
	const float expected[] = {2, 2, 2, 0, 0, 2, 1, 0, 1};
	if(sizeof(expected) != size)
		{
			Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
		}
		else if(memcmp(data, expected, sizeof(expected)))
		{
			Error("Unexpected result!\n");
			PrintMatrix(stderr, data, size, 9);
		}

		called_[CALLED_MaxInput] = true;
}

void ModuleCNN::Vector42(const float * data, size_t size)
{
	if(sizeof(input42) != size)
		{
			Error("Size Mismatch! %lu vs %lu\n", sizeof(input42), size);
		}
		else if(memcmp(data, input42, sizeof(input42)))
		{
			Error("Unexpected result!\n");
			PrintMatrix(stderr, data, size, 9);
		}

		called_[CALLED_Vector42] = true;
}

void ModuleCNN::VectorSplit(const float * data, size_t size)
{
	const float expected[3 * 7] = {
			1.000000, 2.000000, 3.000000, 4.000000, 5.000000, 6.000000, 7.00000,
			2.000000, 3.000000, 4.000000, 5.000000, 6.000000, 7.000000, 8.000000,
			3.000000, 4.000000, 5.000000, 6.000000, 7.000000, 8.000000, 9.000000};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 7);
	}

	called_[CALLED_VectorSplit] = true;
}

void ModuleCNN::CCMaxPool(const float * data, size_t size)
{
	const float expected[4 * 4] = {
			1221.000000, 1311.000000, 1401.000000, 1491.000000,
			2121.000000, 2211.000000, 2301.000000, 2391.000000,
			3021.000000, 3111.000000, 3201.000000, 3291.000000,
			3921.000000, 4011.000000, 4101.000000, 4191.000000};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 4);
	}

	called_[CALLED_CCMaxPool] = true;
}

const float * ModuleCNN::VectorInput(size_t identifier, size_t size)
{
	switch(identifier)
	{
	case 21:
		if(sizeof(input21) != size)
		{
			Error("Unexpected size!\n");
		}
		else
		{
			return input21;
		}
		break;

	case 42:
		if(sizeof(input42) != size)
		{
			Error("Unexpected size!\n");
		}
		else
		{
			return input42;
		}
		break;

	default:
		Error("Unexpected identifier %lu!\n", identifier);
		break;
	}

	return nullptr; // should not be reached
}

void ModuleCNN::CC(const float * data, size_t size)
{
	const float expected[8 * 8] = {
			726.000000, 771.000000, 816.000000, 861.000000, 906.000000, 951.000000, 996.000000, 1041.000000,
			1176.000000, 1221.000000, 1266.000000, 1311.000000, 1356.000000, 1401.000000, 1446.000000, 1491.000000,
			1626.000000, 1671.000000, 1716.000000, 1761.000000, 1806.000000, 1851.000000, 1896.000000, 1941.000000,
			2076.000000, 2121.000000, 2166.000000, 2211.000000, 2256.000000, 2301.000000, 2346.000000, 2391.000000,
			2526.000000, 2571.000000, 2616.000000, 2661.000000, 2706.000000, 2751.000000, 2796.000000, 2841.000000,
			2976.000000, 3021.000000, 3066.000000, 3111.000000, 3156.000000, 3201.000000, 3246.000000, 3291.000000,
			3426.000000, 3471.000000, 3516.000000, 3561.000000, 3606.000000, 3651.000000, 3696.000000, 3741.000000,
			3876.000000, 3921.000000, 3966.000000, 4011.000000, 4056.000000, 4101.000000, 4146.000000, 4191.000000};

	if(sizeof(expected) != size)
	{
		Error("Size Mismatch! %lu vs %lu\n", sizeof(expected), size);
	}
	else if(memcmp(data, expected, sizeof(expected)))
	{
		Error("Unexpected result!\n");
		PrintMatrix(stderr, data, size, 8);
	}

	called_[CALLED_CC] = true;
}

ModuleCNN::ModuleCNN() {
	ModuleCNNPt = this;

	DacModuleCNNOutputCallbackcc_Register(&cc);
	DacModuleCNNOutputCallbackccMaxPool_Register(&ccMaxPool);
	DacModuleCNNOutputCallbackvectorSplit_Register(&vectorSplit);
	DacModuleCNNOutputCallbackvector21_Register(&vector21);
	DacModuleCNNOutputCallbackvector42_Register(&vector42);
	DacModuleCNNOutputCallbackvectorMaxInput_Register(&maxInput);

	DacModuleCNNInputCallbackvector_Register(&vectorInput);
}

void ModuleCNN::Execute(size_t threadsNrOf)
{
	ThreadsNrOf_ = threadsNrOf;

	DacModuleCNNRun(ThreadsNrOf_);

	for(size_t call = 0; call < sizeof(called_) / sizeof(called_[0]); call++)
	{
		if(false == called_[call])
		{
			Error("Not all callbacks executed: Missing %lu!\n", call);
		}
	}
}
