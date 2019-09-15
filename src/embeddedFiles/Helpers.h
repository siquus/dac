/*
 * Helpers.h
 *
 *  Created on: Jul 17, 2019
 *      Author: derommo
 */

#ifndef SRC_HELPERS_H_
#define SRC_HELPERS_H_

#include "Nodes.h"

extern void addPossiblyDeferredJob(const node_t* node);
extern void * threadFunction(void * arg);
extern void StartThreads();
extern void JoinThreads();

#endif /* SRC_HELPERS_H_ */
