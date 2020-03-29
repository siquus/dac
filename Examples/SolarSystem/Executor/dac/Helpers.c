/*
 * Helpers.c
 *
 *  Created on: Jul 17, 2019
 *      Author: derommo
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>

#include "error_functions.h"

#include "Helpers.h"

#define DPRINTF(...) \
	printf(__VA_ARGS__); \
	fflush(stdout);

typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	node_t * jobs[42];
	node_t * deferredJobs[42];
	uint16_t jobsNrOf;
	uint16_t deferredJobsNrOf;
} jobPool_t;

typedef struct {
	pthread_t * pthreads;
	atomic_uchar * threadActive;
	size_t threadsNrOf;
	jobPool_t jobPool;
} threads_t;

typedef struct {
	threads_t * threads;
	uint16_t arrayPos;
} threadInit_t;

static const uint16_t ALL_JOBS_COMPLETED = UINT16_MAX;

static void addJobWithinMutex(threads_t * threads, const node_t* node)
{
	if(sizeof(threads->jobPool.jobs) / sizeof(threads->jobPool.jobs[0]) > threads->jobPool.jobsNrOf)
	{
		threads->jobPool.jobs[threads->jobPool.jobsNrOf] = node;
		threads->jobPool.jobsNrOf++;
	}
	else if(ALL_JOBS_COMPLETED == threads->jobPool.jobsNrOf)
	{
		fatal("Program terminated before being done! Node%u missing!", node->id);
	}
	else
	{
		fatal("Job Pool does not have enough slots!");
	}
}

static void addJob(threads_t * threads, const node_t* node)
{
	int mutexLockRet;
	mutexLockRet = pthread_mutex_lock(&threads->jobPool.mutex);
	if(0 != mutexLockRet)
	{
		errExitEN(mutexLockRet, "pthread_mutex_lock");
	}

	addJobWithinMutex(threads, node);

	int mutexUnlockRet;
	mutexUnlockRet = pthread_mutex_unlock(&threads->jobPool.mutex);
	if(0 != mutexUnlockRet)
	{
		errExitEN(mutexUnlockRet, "pthread_mutex_unlock");
	}
}

static uint8_t allChildrenConsumedParent(const node_t* parent)
{
	// Go through all children and check that they have already consumed this child
	for(uint32_t child = 0; child < parent->childrenNrOf; child++)
	{
		if(parent->children[child]->exeCnt < parent->exeCnt)
		{
			return 0;
		}
	}

	return 1;
}

static void addPossiblyDeferredJobWithinMutex(threads_t * threads, const node_t* node)
{
	if(allChildrenConsumedParent(node))
	{
		addJobWithinMutex(threads, node);
	}
	else
	{
		// Defer this job, as parents aren't ready!
		if(sizeof(threads->jobPool.deferredJobs) / sizeof(threads->jobPool.deferredJobs[0]) > threads->jobPool.deferredJobsNrOf)
		{
			threads->jobPool.deferredJobs[threads->jobPool.deferredJobsNrOf] = node;
			threads->jobPool.deferredJobsNrOf++;
		}
		else
		{
			fatal("Deferred Job Pool does not have enough slots!");
		}
	}
}

void addPossiblyDeferredJob(threads_t * threads, const node_t* node)
{
	int mutexLockRet;
	mutexLockRet = pthread_mutex_lock(&threads->jobPool.mutex);
	if(0 != mutexLockRet)
	{
		errExitEN(mutexLockRet, "pthread_mutex_lock");
	}

	addPossiblyDeferredJobWithinMutex(threads, node);

	int mutexUnlockRet;
	mutexUnlockRet = pthread_mutex_unlock(&threads->jobPool.mutex);
	if(0 != mutexUnlockRet)
	{
		errExitEN(mutexUnlockRet, "pthread_mutex_unlock");
	}
}

void * threadFunction(void * arg)
{
	threadInit_t * init = (threadInit_t *) arg;

	const uint16_t threadArrayIndex = init->arrayPos;
	threads_t * threads = init->threads;

	node_t * nodeJob = NULL;

	instructionParam_t instructionParam = {
			.instance = threads,
			.addPossiblyDeferredNode = addPossiblyDeferredJob
	};

	while(1)
	{
		int mutexLockRet;
		mutexLockRet = pthread_mutex_lock(&threads->jobPool.mutex);
		if(0 != mutexLockRet)
		{
			errExitEN(mutexLockRet, "pthread_mutex_lock");
		}

		// In case we generate new jobs for other threads with this:
		uint8_t jobsWereEmptyBefore = 0;
		if(0 == threads->jobPool.jobsNrOf)
		{
			jobsWereEmptyBefore = 1;
		}

		// Update from last loop run
		if(NULL != nodeJob)
		{
			nodeJob->exeCnt++;
			DPRINTF("Thread %2u: updated node %u exe cnt to %u, added nodes {",
					threadArrayIndex, nodeJob->id, nodeJob->exeCnt);

			// Have any children become available for execution?
			for(uint32_t child = 0; child < nodeJob->childrenNrOf; child++)
			{
				uint8_t readyForExe = 1;
				node_t* pChild = nodeJob->children[child];

				// Go through all parents and check if they already executed
				for(uint32_t parent = 0; parent < pChild->parentsNrOf; parent++)
				{
					if(pChild->parents[parent]->exeCnt > pChild->exeCnt + 1)
					{
						fatal("Parent Node%u was executed once without its child Node%u afterwards!",
								pChild->parents[parent]->id,
								pChild->id);
					}

					if(pChild->parents[parent]->exeCnt != pChild->exeCnt + 1)
					{
						readyForExe = 0;
						break;
					}
				}

				if(!readyForExe)
				{
					continue; // NOTE: Below code assumes parents checked out!
				}

				if(!allChildrenConsumedParent(pChild))
				{
					// Defer this job, as parents are ready!
					if(sizeof(threads->jobPool.deferredJobs) / sizeof(threads->jobPool.deferredJobs[0]) > threads->jobPool.deferredJobsNrOf)
					{
						DPRINTF("(%u) ", pChild->id);

						threads->jobPool.deferredJobs[threads->jobPool.deferredJobsNrOf] = pChild;
						threads->jobPool.deferredJobsNrOf++;
					}
					else
					{
						fatal("Deferred Job Pool does not have enough slots!");
					}
				}
				else // Child is ready for execution!
				{
					DPRINTF("%u ", pChild->id);
					addJobWithinMutex(threads, pChild);
				}
			}

			// Check out all deferred jobs
			for(int defJob = threads->jobPool.deferredJobsNrOf - 1; defJob >= 0; defJob--)
			{
				node_t * pDefJob = threads->jobPool.deferredJobs[defJob];
				if(!allChildrenConsumedParent(pDefJob))
				{
					continue;
				}

				addJobWithinMutex(threads, pDefJob);
				DPRINTF("%u ", pDefJob->id);

				for(uint16_t cpyJob = threads->jobPool.deferredJobsNrOf - 1; cpyJob > defJob; cpyJob--)
				{
					threads->jobPool.deferredJobs[cpyJob - 1] = threads->jobPool.deferredJobs[cpyJob];
				}

				threads->jobPool.deferredJobsNrOf--;
			}

			DPRINTF("} to job list\n");

			if(0 == threads->jobPool.jobsNrOf)
			{
				// Any other thread still working or are we out of work?
				uint8_t stillWorking = 0;
				for(uint16_t thread = 0; thread < threads->threadsNrOf; thread++)
				{
					if(threadArrayIndex == thread)
					{
						continue; // this is us
					}

					if(threads->threadActive[thread])
					{
						stillWorking = 1;
						break;
					}
				}

				if(!stillWorking)
				{
					// Program is done: Let other threads know and return
					DPRINTF("Thread %2u: executed last instruction!\n", threadArrayIndex);

					goto SIGNAL_DONE_AND_TERMINATE;
				}
			}
		}

		while (0 == threads->jobPool.jobsNrOf)
		{
			threads->threadActive[threadArrayIndex] = 0;

			int waitRet = pthread_cond_wait(&threads->jobPool.condition, &threads->jobPool.mutex);
			if(waitRet != 0)
			{
				errExitEN(waitRet, "pthread_cond_wait");
			}
		}

		if(ALL_JOBS_COMPLETED == threads->jobPool.jobsNrOf)
		{
			goto SIGNAL_DONE_AND_TERMINATE;
		}
		else
		{
			nodeJob = threads->jobPool.jobs[threads->jobPool.jobsNrOf - 1];
			threads->jobPool.jobsNrOf--;

			threads->threadActive[threadArrayIndex] = 1;

			DPRINTF("Thread %2u: picking up Node %2u. ", threadArrayIndex, nodeJob->id);
			DPRINTF("%2u Nodes remaining: ", threads->jobPool.jobsNrOf);

			for(uint32_t node = 0; node < threads->jobPool.jobsNrOf; node++)
			{
				DPRINTF("%u, ", (threads->jobPool.jobs[node])->id);
			}

			DPRINTF("\tThread active: ");
			for(uint16_t thread = 0; thread < threads->threadsNrOf; thread++)
			{
				if(threads->threadActive[thread])
				{
					DPRINTF("1 ");
				}
				else
				{
					DPRINTF("0 ");
				}
			}
			DPRINTF("\n\n");
		}

		uint8_t signalJobsAvailable = 0;
		if(jobsWereEmptyBefore && threads->jobPool.jobsNrOf)
		{
			signalJobsAvailable = 1;
		}

		int mutexUnlockRet;
		mutexUnlockRet = pthread_mutex_unlock(&threads->jobPool.mutex);
		if(0 != mutexUnlockRet)
		{
			errExitEN(mutexUnlockRet, "pthread_mutex_unlock");
		}

		if(signalJobsAvailable)
		{
			int condSignalReturn = pthread_cond_signal(&threads->jobPool.condition); // Wake sleeping consumer
			if (condSignalReturn != 0)
			{
				errExitEN(condSignalReturn, "pthread_cond_signal");
			}
		}

		// Run instruction
		nodeJob->instruction(&instructionParam);
	}

	SIGNAL_DONE_AND_TERMINATE:
	threads->jobPool.jobsNrOf = ALL_JOBS_COMPLETED;

	int mutexUnlockRet;
	mutexUnlockRet = pthread_mutex_unlock(&threads->jobPool.mutex);
	if(0 != mutexUnlockRet)
	{
		errExitEN(mutexUnlockRet, "pthread_mutex_unlock");
	}

	int condSignalReturn = pthread_cond_signal(&threads->jobPool.condition); // Wake sleeping consumer
	if (condSignalReturn != 0)
	{
		errExitEN(condSignalReturn, "pthread_cond_signal");
	}

	return NULL;
}

void StartThreads(void ** instance, size_t threadsNrOf, jobPoolInit_t * jobPoolInit)
{
	// Why this weird way of assigning the pointer? Because otherwise we can't hand it over to
	// to JoinThreads. See https://www.viva64.com/en/b/0576/
	*instance = malloc(sizeof(threads_t));
	if(NULL == *instance)
	{
		fatal("Could not malloc threads_t!\n");
	}

	threads_t * threads = (threads_t*) *instance;

	threads->threadsNrOf = threadsNrOf;

	pthread_t * pthreads = malloc(sizeof(pthread_t) * threadsNrOf);
	if(NULL == pthreads)
	{
		fatal("Could not malloc pthread_t!\n");
	}

	threads->pthreads = pthreads;

	atomic_uchar * threadActive = malloc(sizeof(atomic_uchar) * threadsNrOf);
	if(NULL == threadActive)
	{
		fatal("Could not malloc atomic_uchar!\n");
	}

	threads->threadActive = threadActive;

	for(size_t thread = 0; thread < threads->threadsNrOf; thread++)
	{
		threads->threadActive[thread] = 0;
	}

	pthread_mutex_init(&threads->jobPool.mutex, NULL);
	pthread_cond_init(&threads->jobPool.condition, NULL);

	if(sizeof(threads->jobPool.jobs) / sizeof(threads->jobPool.jobs[0]) < jobPoolInit->NodesNrOf)
	{
		fatal("Too many jobs to initialize job pool!");
	}

	DPRINTF("Job pool is initialized with nodes {");
	for(uint16_t node = 0; node < jobPoolInit->NodesNrOf; node++)
	{
		threads->jobPool.jobs[node] = jobPoolInit->Nodes[node];
		DPRINTF("%u ", jobPoolInit->Nodes[node]->id);
	}
	threads->jobPool.jobsNrOf = jobPoolInit->NodesNrOf;
	DPRINTF("}\n");

	threads->jobPool.deferredJobsNrOf = 0;

	DPRINTF("Starting %lu threads\n", threads->threadsNrOf);

	threadInit_t* threadInits = malloc(sizeof(threadInit_t) * threads->threadsNrOf);
	if(NULL == threadInits)
	{
		fatal("Could not malloc threadInit_t!\n");
	}

	for(uint16_t arrayPos = 0; arrayPos < threads->threadsNrOf; arrayPos++)
	{
		threadInits[arrayPos].threads = threads;
		threadInits[arrayPos].arrayPos = arrayPos;

		int threadCreateRet;
		threadCreateRet = pthread_create(&threads->pthreads[arrayPos], NULL, threadFunction, &threadInits[arrayPos]);
		if(0 != threadCreateRet)
		{
			errExitEN(threadCreateRet, "pthread_create");
		}
	}

	return;
}

void JoinThreads(void * instance)
{
	threads_t * threads = (threads_t * ) instance;

	for(uint16_t thread = 0; thread < threads->threadsNrOf; thread++)
	{
		int joinRet;
		joinRet = pthread_join(threads->pthreads[thread], NULL);
		if(0 != joinRet)
		{
			errExitEN(joinRet, "pthread_join");
		}
	}

	// TODO: Free stuff!
}

