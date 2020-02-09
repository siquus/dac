/*
 * Helpers.h
 *
 *  Created on: Jul 17, 2019
 *      Author: derommo
 */

#ifndef SRC_HELPERS_H_
#define SRC_HELPERS_H_

#include "Nodes.h"

extern void * threadFunction(void * arg);
extern void StartThreads(void ** instance, size_t threadsNrOf, jobPoolInit_t * jobPoolInit);
extern void JoinThreads(void * instance);

#endif /* SRC_HELPERS_H_ */
