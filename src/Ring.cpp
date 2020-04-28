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

#include <stdint.h>

#include "Ring.h"

using namespace Algebra;

Ring::type_t Ring::GetSuperiorRing(type_t t1, type_t t2)
{
	if((int) t1 > (int) t2)
	{
		return t1;
	}

	return t2;
}

size_t Ring::GetElementSize(type_t type)
{
	switch(type)
	{
	default: // no break intended
	case None:
		Error("Can't get size of unknown ring!\n");
		return SIZE_MAX;

	case Int32:
		return sizeof(int32_t);

	case Float32:
		return 4;
	}
}
