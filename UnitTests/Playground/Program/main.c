#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dac.h"

const float expectedProduct[] = {210, 294, 378};
const float expectedSum[] = {221, 315, 409};

// typedef void (*DacOutputCallbackProduct_t)(float* pt, size_t size);
static void productCallback(float* pt, size_t size)
{
	if(memcmp(pt, expectedProduct, sizeof(expectedProduct)) || (size != sizeof(expectedProduct)))
	{
		fprintf(stderr, "Unexpected product result!\n");
		fflush(stderr);
		exit(1);
	}
}

static void sumCallback(float* pt, size_t size)
{
	if(memcmp(pt, expectedSum, sizeof(expectedSum)) || (size != sizeof(expectedSum)))
	{
		fprintf(stderr, "Unexpected sum result!\n");
		fflush(stderr);
		exit(1);
	}
}


int main() {
	DacOutputCallbackProduct_Register(productCallback);
	DacOutputCallbackSum_Register(sumCallback);

	DacRun();

	fprintf(stdout, "SUCCESS!!\n");
	fflush(stdout);

	return 0;
}
