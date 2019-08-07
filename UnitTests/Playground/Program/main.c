#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dac.h"

const float expectedProduct[] = {210, 294, 378};
const float expectedSum[] = {221, 315, 409};
const int expectedIsSmaller = 0;

// typedef void (*DacOutputCallbackProduct_t)(float* pt, size_t size);
static void productCallback(const float* pt, size_t size)
{
	if(memcmp(pt, expectedProduct, sizeof(expectedProduct)) || (size != sizeof(expectedProduct)))
	{
		fprintf(stderr, "Unexpected product result!\n");
		fflush(stderr);
		exit(1);
	}
}

static void sumCallback(const float* pt, size_t size)
{
	if(memcmp(pt, expectedSum, sizeof(expectedSum)) || (size != sizeof(expectedSum)))
	{
		fprintf(stderr, "Unexpected sum result!\n");
		fflush(stderr);
		exit(1);
	}
}

static void isSmallerCallback(const int* pt, size_t size)
{
	if(sizeof(int32_t) != size)
	{
		fprintf(stderr, "Unexpected size of isSmaller: Got %lu, expected %lu\n",
				size, sizeof(int32_t));
		fflush(stderr);
		exit(1);
	}

	if(expectedIsSmaller != *pt)
	{
		fprintf(stderr, "Unexpected isSmaller result!\n");
		fflush(stderr);
		exit(1);
	}
}


int main() {
	DacOutputCallbackProduct_Register(productCallback);
	DacOutputCallbackSum_Register(sumCallback);
	DacOutputCallbackSmallerThan_Register(isSmallerCallback);

	DacRun();

	fprintf(stdout, "SUCCESS!!\n");
	fflush(stdout);

	return 0;
}
