#include <stdio.h>
#include <pthread.h>

#include "dac.h"

static pthread_mutex_t stdoutMutex = PTHREAD_MUTEX_INITIALIZER;

// typedef void (*DacOutputCallbackProduct_t)(float* pt, size_t size);
static void resultCallback(float* pt, size_t size)
{
	pthread_mutex_lock(&stdoutMutex);

	printf("Result = ");

	int nrOfFloats = size / sizeof(float);
	for(int elem = 0; elem < nrOfFloats; elem++)
	{
		printf("%f ", pt[elem]);
	}

	printf("\n");
	fflush(stdout);

	pthread_mutex_unlock(&stdoutMutex);
}

int main() {
	DacOutputCallbackProduct_Register(resultCallback);
	DacOutputCallbackSum_Register(resultCallback);

	DacRun();

	return 0;
}
