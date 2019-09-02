#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dac.h"

const float expectedProduct[] = {210, 294, 378};
const float expectedSum[] = {221, 315, 409};
const int expectedIsSmaller = 0;
typedef enum {
	CALLED_PRODUCT = 1 << 0,
	CALLED_SUM = 1 << 1,
	CALLED_IS_SMALLER = 1 << 2,
	CALLED_NROF = 1 << 3,
} called_t;

static called_t called = 0;

// typedef void (*DacOutputCallbackProduct_t)(float* pt, size_t size);
static void productCallback(const float* pt, size_t size)
{
	if(memcmp(pt, expectedProduct, sizeof(expectedProduct)) || (size != sizeof(expectedProduct)))
	{
		fprintf(stderr, "Unexpected product result!\n");
		fflush(stderr);
		exit(1);
	}

	called |= CALLED_PRODUCT;
}

static void sumCallback(const float* pt, size_t size)
{
	if(memcmp(pt, expectedSum, sizeof(expectedSum)) || (size != sizeof(expectedSum)))
	{
		fprintf(stderr, "Unexpected sum result!\n");
		fflush(stderr);
		exit(1);
	}

	called |= CALLED_SUM;
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

	called |= CALLED_IS_SMALLER;
}


int main() {
	DacOutputCallbackProduct_Register(productCallback);
	DacOutputCallbackSum_Register(sumCallback);
	DacOutputCallbackSmallerThan_Register(isSmallerCallback);

	DacRun();

	if(CALLED_NROF - 1 != called)
	{
		fprintf(stderr, "Not all Callbacks called!\n");
		fflush(stderr);
		exit(1);
	}

	fprintf(stdout, "SUCCESS!!\n");
	fflush(stdout);

	return 0;
}
