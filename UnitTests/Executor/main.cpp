#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "error_functions.h"

#include "ModuleContract.h"
#include "ModulePermute.h"
#include "ModuleProduct.h"

#if 0
#include "DacTest.h"
#endif

static const float expectedProduct[] = {210, 294, 378};
static const float expectedSum[] = {221, 315, 409};
static const int expectedIsSmaller = 0;
static const int expectedLoopCnt = 10;
static const float expectedMatrixProduct[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
static const float expectedMatrixKronProduct[] = {2, 4, 6, 8, 10, 12, 14, 16, 18};

#ifdef DEBUG_CALLED
typedef enum {
	CALLED_PRODUCT = 1 << 0,
	CALLED_SUM = 1 << 1,
	CALLED_IS_SMALLER = 1 << 2,
	CALLED_WHILE_DONE = 1 << 3,
	CALLED_MATRIX_PROD = 1 << 4,
	CALLED_MATRIX_KRON_PROD = 1 << 5,
	CALLED_NROF = 1 << 6,
} called_t;

static called_t called = 0;
#endif // DEBUG_CALLED

static void productCallback(const float* pt, size_t size)
{
	if(memcmp(pt, expectedProduct, sizeof(expectedProduct)) || (size != sizeof(expectedProduct)))
	{
		fprintf(stderr, "Unexpected product result!\n");
		fflush(stderr);
		exit(1);
	}

#if DEBUG_CALLED
	called |= CALLED_PRODUCT;
#endif // DEBUG_CALLED
}

static void matrixProductCallback(const float* pt, size_t size)
{
	if(memcmp(pt, expectedMatrixProduct, sizeof(expectedMatrixProduct)) || (size != sizeof(expectedMatrixProduct)))
	{
		fprintf(stderr, "Unexpected product result!\n");
		fflush(stderr);
		exit(1);
	}

#ifdef DEBUG_CALLED
	called |= CALLED_MATRIX_PROD;
#endif // DEBUG_CALLED
}

static void matrixKronProductCallback(const float* pt, size_t size)
{
	if(memcmp(pt, expectedMatrixKronProduct, sizeof(expectedMatrixKronProduct)) || (size != sizeof(expectedMatrixKronProduct)))
	{
		fprintf(stderr, "Unexpected product result!\n");
		fflush(stderr);
		exit(1);
	}

#ifdef DEBUG_CALLED
	called |= CALLED_MATRIX_KRON_PROD;
#endif // DEBUG_CALLED
}

static void sumCallback(const float* pt, size_t size)
{
	if(memcmp(pt, expectedSum, sizeof(expectedSum)) || (size != sizeof(expectedSum)))
	{
		fprintf(stderr, "Unexpected sum result!\n");
		fflush(stderr);
		exit(1);
	}

#ifdef DEBUG_CALLED
	called |= CALLED_SUM;
#endif // DEBUG_CALLED
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

#ifdef DEBUG_CALLED
	called |= CALLED_IS_SMALLER;
#endif // DEBUG_CALLED
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
#ifdef DEBUG_CALLED
		called |= CALLED_WHILE_DONE;
#endif // DEBUG_CALLED
	}
}

int main() {

	ModuleContract moduleContract;
	moduleContract.Execute(4);
	if(!moduleContract.Success())
	{
		fatal("Not all tests passed!\n");
	}

	ModulePermute modulePermute;
	modulePermute.Execute(4);
	if(!modulePermute.Success())
	{
		fatal("Not all tests passed!\n");
	}

	ModuleProduct moduleProduct;
	moduleProduct.Execute(4);
	if(!moduleProduct.Success())
	{
		fatal("Not all tests passed!\n");
	}

#if 0
	DacTestOutputCallbackProduct_Register(productCallback);
	DacTestOutputCallbackSum_Register(sumCallback);
	DacTestOutputCallbackSmallerThan_Register(isSmallerCallback);
	DacTestOutputCallbackmatrixProd_Register(matrixProductCallback);
	DacTestOutputCallbackmatrixKronProd_Register(matrixKronProductCallback);
	DacTestOutputCallbackWhile_Register(whileCallback);

	DacTestRun(4);

#ifdef DEBUG_CALLED
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
#endif // DEBUG_CALLED
#endif

	fprintf(stdout, "SUCCESS!!\n");
	fflush(stdout);


	return 0;
}
