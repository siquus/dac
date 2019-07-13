#include <stdio.h>

#include "dac.h"

// typedef void (*DacOutputCallbackProduct_t)(float* pt, size_t size);
static void resultCallback(float* pt, size_t size)
{
	printf("Result = ");

	int nrOfFloats = size / sizeof(float);
	for(int elem = 0; elem < nrOfFloats; elem++)
	{
		printf("%f ", pt[elem]);
	}

	printf("\n");
}

int main() {
	DacOutputCallbackProduct_Register(resultCallback);
	DacRun();

	return 0;
}
