/*
 * Helpers.h
 *
 *  Created on: Jul 17, 2019
 *      Author: derommo
 */

#ifndef SRC_HELPERS_H_
#define SRC_HELPERS_H_

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
		.jobs = {NULL},
		.jobsNrOf = 0
};

void * threadFunction(void * arg)
{
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
		}

		while (0 == nodeJobPool.jobsNrOf)
		{
			int waitRet = pthread_cond_wait(&nodeJobPool.condition, &nodeJobPool.mutex);
			if(waitRet != 0)
			{
				errExitEN(waitRet, "pthread_cond_wait");
			}
		}

		if(ALL_JOBS_COMPLETED == nodeJobPool.jobsNrOf)
		{
			break; // done
		}
		else
		{
			nodeJob = nodeJobPool.jobs[nodeJobPool.jobsNrOf - 1];
			nodeJobPool.jobsNrOf--;
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

	return NULL;
}

#endif /* SRC_HELPERS_H_ */
