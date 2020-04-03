/*
 * Helpers.h
 *
 *  Created on: Jul 17, 2019
 *      Author: derommo
 */

#ifndef SRC_HELPERS_H_
#define SRC_HELPERS_H_

#include <stdatomic.h>
#include <pthread.h>

#define NODE_T_MAX_EDGE_NUMBER 42u

typedef void (*instruction_t)(struct instructionParam_s * param);

typedef struct node_s {
	instruction_t instruction;
	const struct node_s * parents[NODE_T_MAX_EDGE_NUMBER];
	struct node_s * children[NODE_T_MAX_EDGE_NUMBER];
	uint32_t exeCnt;
	const uint16_t parentsNrOf;
	const uint16_t childrenNrOf;
	const uint16_t id;
} node_t;

typedef void (*PushNode_t)(void * instance, struct node_s * node);

typedef struct instructionParam_s {
	void * Instance;
	PushNode_t PushNode;
} instructionParam_t;

typedef struct jobPoolInit_s {
	node_t ** Nodes;
	size_t NodesNrOf;
} jobPoolInit_t;

extern void * threadFunction(void * arg);
extern void StartThreads(void ** instance, size_t threadsNrOf, jobPoolInit_t * jobPoolInit);
extern void JoinThreads(void * instance);

#endif /* SRC_HELPERS_H_ */
