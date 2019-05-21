/*
 * Ring.cpp
 *
 *  Created on: May 17, 2019
 *      Author: derommo
 */

#include <stdint.h>
#include "Ring.h"

using namespace Algebra;

Ring::Type Algebra::Ring::GetSuperiorRing(Type t1, Type t2)
{
	if((int) t1 > (int) t2)
	{
		return t1;
	}

	return t2;
}
