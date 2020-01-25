#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "dac.h"

static const float expectedProduct[] = {210, 294, 378};
static const float expectedSum[] = {221, 315, 409};
static const int expectedIsSmaller = 0;
static const int expectedLoopCnt = 10;
static const float expectedMatrixProduct[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

typedef enum {
	CALLED_PRODUCT = 1 << 0,
	CALLED_SUM = 1 << 1,
	CALLED_IS_SMALLER = 1 << 2,
	CALLED_WHILE_DONE = 1 << 3,
	CALLED_MATRIX_PROD = 1 << 4,
	CALLED_NROF = 1 << 5,
} called_t;

static called_t called = 0;

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

static void matrixProductCallback(const float* pt, size_t size)
{
	if(memcmp(pt, expectedMatrixProduct, sizeof(expectedMatrixProduct)) || (size != sizeof(expectedMatrixProduct)))
	{
		fprintf(stderr, "Unexpected product result!\n");
		fflush(stderr);
		exit(1);
	}

	called |= CALLED_MATRIX_PROD;
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

static void whileCallback(const float * pt, size_t size)
{
	const uint32_t expectedIterations = 10;
	static uint32_t loopIterations = 0;
	loopIterations++;

	if(expectedIterations < loopIterations)
	{
		fatal("have more than %u loop iterations!\n", expectedIterations);
	}

	float expectedVec[3] = {10 - loopIterations, 10 - loopIterations, 10 - loopIterations};

	if(sizeof(expectedVec) != size)
	{
		fprintf(stderr, "Unexpected while var size\n");
		fflush(stderr);
		exit(1);
	}

	if(memcmp(pt, expectedVec, sizeof(expectedVec)))
	{
		fprintf(stderr, "Unexpected while var: expected (%f, %f, %f), got (%f, %f, %f)\n",
				expectedVec[0], expectedVec[1], expectedVec[2],
				pt[0], pt[1], pt[2]);

		fflush(stderr);
		exit(1);
	}

	if(expectedIterations == loopIterations)
	{
		called |= CALLED_WHILE_DONE;
	}
}

int main() {
	DacOutputCallbackProduct_Register(productCallback);
	DacOutputCallbackSum_Register(sumCallback);
	DacOutputCallbackSmallerThan_Register(isSmallerCallback);
	DacOutputCallbackmatrixProd_Register(matrixProductCallback);
	DacOutputCallbackWhile_Register(whileCallback);

	DacRun();

	if(CALLED_NROF - 1 != called)
	{
		fprintf(stderr, "Not all Callbacks called! Missing: ");
		for(called_t bit = 0; (1 << bit) < CALLED_NROF; bit++)
		{
			if(0 == (called & (1 << bit)))
			{
				fprintf(stderr, "%u ", bit);
			}
		}
		fprintf(stderr, "\n");

		fflush(stderr);
		exit(1);
	}

	fprintf(stdout, "SUCCESS!!\n");
	fflush(stdout);

	return 0;
}
