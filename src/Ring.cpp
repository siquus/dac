/*
 * Ring.cpp
 *
 *  Created on: May 17, 2019
 *      Author: derommo
 */

#include <stdint.h>
#include "Ring.h"

using namespace Algebra;

Ring::type_t Algebra::Ring::GetSuperiorRing(type_t t1, type_t t2)
{
	if((int) t1 > (int) t2)
	{
		return t1;
	}

	return t2;
}
