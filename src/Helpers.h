/*
 * Helpers.h
 *
 *  Created on: Jul 17, 2019
 *      Author: derommo
 */

#ifndef SRC_HELPERS_H_
#define SRC_HELPERS_H_

#include <stdio.h>
#include <stdatomic.h>

#define DPRINTF(...) \
	printf(__VA_ARGS__); \
	fflush(stdout);

static pthread_t threads[THREADS_NROF];
static atomic_uchar threadActive[THREADS_NROF] = {0};

const uint16_t ALL_JOBS_COMPLETED = UINT16_MAX;
typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	node_t * jobs[42];
	uint16_t jobsNrOf;
} nodeJobPool_t;

static nodeJobPool_t nodeJobPool = {
		.mutex = PTHREAD_MUTEX_INITIALIZER,
		.condition = PTHREAD_COND_INITIALIZER,
		.jobs = JOB_POOL_INIT,
		.jobsNrOf = JOB_POOL_INIT_NROF
};

void * threadFunction(void * arg)
{
	uint16_t threadArrayIndex = *((uint16_t*) arg);

	node_t * nodeJob = NULL;

	while(1)
	{
		int mutexLockRet;
		mutexLockRet = pthread_mutex_lock(&nodeJobPool.mutex);
		if(0 != mutexLockRet)
		{
			errExitEN(mutexLockRet, "pthread_mutex_lock");
		}

		// Update from last loop run
		uint8_t jobsWereEmptyBefore = 0; // In case we generate new jobs for other threads with this.
		if(NULL != nodeJob)
		{
			nodeJob->exeCnt++;

			// Have any children become available for execution?
			// Go through all children's parents and check
			uint8_t readyForExe = 1;
			for(uint32_t child = 0; child < nodeJob->childrenNrOf; child++)
			{
				node_t* pChild = nodeJob->children[child];
				for(uint32_t parent = 0; parent < pChild->parentsNrOf; parent++)
				{
					if(pChild->parents[parent]->exeCnt > pChild->exeCnt + 1)
					{
						fatal("Parent was executed once without its child afterwards!");
					}

					if(pChild->parents[parent]->exeCnt != pChild->exeCnt + 1)
					{
						readyForExe = 0;
						break;
					}
				}

				if(readyForExe)
				{
					if(0 == nodeJobPool.jobsNrOf)
					{
						jobsWereEmptyBefore = 1;
					}

					if(sizeof(nodeJobPool.jobs) / sizeof(nodeJobPool.jobs[0]) > nodeJobPool.jobsNrOf)
					{
						nodeJobPool.jobs[nodeJobPool.jobsNrOf] = pChild;
						nodeJobPool.jobsNrOf++;
					}
					else
					{
						fatal("Job Pool does not have enough slots!");
					}
				}
			}

			if(0 == nodeJobPool.jobsNrOf)
			{
				// Any other thread still working or are we out of work?
				uint8_t stillWorking = 0;
				for(uint16_t thread = 0; thread < sizeof(threads) / sizeof(threads[0]); thread++)
				{
					if(threadArrayIndex == thread)
					{
						continue; // this is us
					}

					if(threadActive[thread])
					{
						stillWorking = 1;
						break;
					}
				}

				if(!stillWorking)
				{
					// Program is done: Let other threads know and return
					DPRINTF("Thread %u executed last instruction!\n", threadArrayIndex);

					goto SIGNAL_DONE_AND_TERMINATE;
				}
			}
		}

		while (0 == nodeJobPool.jobsNrOf)
		{
			threadActive[threadArrayIndex] = 0;

			int waitRet = pthread_cond_wait(&nodeJobPool.condition, &nodeJobPool.mutex);
			if(waitRet != 0)
			{
				errExitEN(waitRet, "pthread_cond_wait");
			}
		}

		if(ALL_JOBS_COMPLETED == nodeJobPool.jobsNrOf)
		{
			goto SIGNAL_DONE_AND_TERMINATE;
		}
		else
		{
			nodeJob = nodeJobPool.jobs[nodeJobPool.jobsNrOf - 1];
			nodeJobPool.jobsNrOf--;

			threadActive[threadArrayIndex] = 1;

			DPRINTF("Thread %2u picking up Node %2u. ", threadArrayIndex, nodeJob->id);
			DPRINTF("%2u Nodes remaining: ", nodeJobPool.jobsNrOf);

			for(uint32_t node = 0; node < nodeJobPool.jobsNrOf; node++)
			{
				DPRINTF("%u, ", (nodeJobPool.jobs[node])->id);
			}

			DPRINTF("\tThread active: ");
			for(uint16_t thread = 0; thread < sizeof(threadActive) / sizeof(threadActive[0]); thread++)
			{
				if(threadActive[thread])
				{
					DPRINTF("1 ");
				}
				else
				{
					DPRINTF("0 ");
				}
			}
			DPRINTF("\n");
		}

		uint8_t signalJobsAvailable = 0;
		if(jobsWereEmptyBefore && nodeJobPool.jobsNrOf)
		{
			signalJobsAvailable = 1;
		}

		int mutexUnlockRet;
		mutexUnlockRet = pthread_mutex_unlock(&nodeJobPool.mutex);
		if(0 != mutexUnlockRet)
		{
			errExitEN(mutexUnlockRet, "pthread_mutex_unlock");
		}

		if(signalJobsAvailable)
		{
			int condSignalReturn = pthread_cond_signal(&nodeJobPool.condition); // Wake sleeping consumer
			if (condSignalReturn != 0)
			{
				errExitEN(condSignalReturn, "pthread_cond_signal");
			}
		}

		// Run instruction
		nodeJob->instruction();
	}

	SIGNAL_DONE_AND_TERMINATE:
	nodeJobPool.jobsNrOf = ALL_JOBS_COMPLETED;

	int mutexUnlockRet;
	mutexUnlockRet = pthread_mutex_unlock(&nodeJobPool.mutex);
	if(0 != mutexUnlockRet)
	{
		errExitEN(mutexUnlockRet, "pthread_mutex_unlock");
	}

	int condSignalReturn = pthread_cond_signal(&nodeJobPool.condition); // Wake sleeping consumer
	if (condSignalReturn != 0)
	{
		errExitEN(condSignalReturn, "pthread_cond_signal");
	}

	return NULL;
}

void StartThreads()
{
	DPRINTF("Starting %lu threads\n", sizeof(threads) / sizeof(threads[0]));

	for(uint16_t thread = 0; thread < sizeof(threads) / sizeof(threads[0]); thread++)
	{
		int threadCreateRet;
		threadCreateRet = pthread_create(&threads[thread], NULL, threadFunction, &thread);
		if(0 != threadCreateRet)
		{
			errExitEN(threadCreateRet, "pthread_create");
		}
	}
}

void JoinThreads()
{
	for(uint16_t thread = 0; thread < sizeof(threads) / sizeof(threads[0]); thread++)
	{
		int joinRet;
		joinRet = pthread_join(threads[thread], NULL);
		if(0 != joinRet)
		{
			errExitEN(joinRet, "pthread_join");
		}
	}
}

#endif /* SRC_HELPERS_H_ */
