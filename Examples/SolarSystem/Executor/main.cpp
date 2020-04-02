#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "DacSolarSystem.h"

static void StateCallbac(const float* pt, size_t size)
{

}

int main()
{
	DacSolarSystemOutputCallbackNewState_Register(&StateCallbac);
	DacSolarSystemRun(4);

	return 0;
}
