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

#ifndef SRC_RING_H_
#define SRC_RING_H_

#include <vector>

#include "GlobalDefines.h"

namespace Algebra {
namespace Ring {

// Below Type ordering matters: Larger enum value means superior ring
typedef enum {
	None,
	Int32,
	Float32,
} type_t;

extern type_t GetSuperiorRing(type_t t1, type_t t2);
extern size_t GetElementSize(type_t type);

template<typename inType>
bool IsCompatible(type_t type, const std::vector<inType>  &dataVector __attribute__((unused))) // using dataVector only to get its type
{
	switch(type)
	{
	case Ring::Float32:
		if(!std::is_same<inType, float>::value)
		{
			Error("Type mismatch\n");
			return false;
		}
		return true;

	case Ring::Int32:
		if(!std::is_same<inType, int32_t>::value)
		{
			Error("Type mismatch\n");
			return false;
		}
		return true;

	default:
		Error("Type mismatch\n");
		return false;
	}

		Error("Should not be reached!\n");
		return false;
}

template<typename inType>
bool IsCompatible(type_t type, const inType &data __attribute__((unused))) // using dataVector only to get its type
{
	switch(type)
	{
	case Ring::Float32:
		if(!std::is_same<inType, float>::value)
		{
			Error("Type mismatch\n");
			return false;
		}
		return true;

	case Ring::Int32:
		if(!std::is_same<inType, int32_t>::value)
		{
			Error("Type mismatch\n");
			return false;
		}
		return true;

	default:
		Error("Type mismatch\n");
		return false;
	}

		Error("Should not be reached!\n");
		return false;
}

}
}
#endif /* SRC_RING_H_ */
