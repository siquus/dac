/*
 * Ring.cpp
 *
 *  Created on: May 17, 2019
 *      Author: derommo
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
