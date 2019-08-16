/*
 * Helpers.h
 *
 *  Created on: Jul 17, 2019
 *      Author: derommo
 */

#ifndef SRC_HELPERS_H_
#define SRC_HELPERS_H_

void * threadFunction(void * arg)
{
	while(1)
	{
		int pcMutexLockRet;
		pcMutexLockRet = pthread_mutex_lock(&programCounterMutex);
		if(0 != pcMutexLockRet)
		{
			errExitEN(pcMutexLockRet, "pthread_mutex_lock");
		}

		instruction_t instruction = NULL;
		if(sizeof(instructionList) / sizeof(instructionList[0]) < programCounter)
		{
			// Fetch instruction and increase pc
			instruction = instructionList[programCounter];
			programCounter++;
		}

		int pcMutexUnlockRet;
		pcMutexUnlockRet = pthread_mutex_unlock(&programCounterMutex);
		if(0 != pcMutexUnlockRet)
		{
			errExitEN(pcMutexUnlockRet, "pthread_mutex_unlock");
		}

		// Check if we got a new instruction
		if(NULL == instruction)
		{
			break; // program done.
		}

		// Run instruction
		instruction();
	}

	return NULL;
}

#endif /* SRC_HELPERS_H_ */
