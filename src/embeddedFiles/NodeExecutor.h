/*
 * This file is part of
 * Distributed Algebraic Computations (https://github.com/siquus/dac)
 *
 * GPL-3 (or later)
 *
 * Copyright (C) 2020  Patrik Omland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SRC_NODEEXECUTOR_H_
#define SRC_NODEEXECUTOR_H_

#include <stdatomic.h>
#include <pthread.h>
#include <stdint.h>

#define NODE_T_MAX_EDGE_NUMBER 42u

typedef struct node_s {
	void (*instruction)(void * instance, void (*PushNode)(void * instance, struct node_s * node));
	const struct node_s * parents[NODE_T_MAX_EDGE_NUMBER];
	struct node_s * children[NODE_T_MAX_EDGE_NUMBER];
	uint32_t exeCnt;
	const uint16_t parentsNrOf;
	const uint16_t childrenNrOf;
	const uint16_t id;
} node_t;

typedef struct jobPoolInit_s {
	node_t ** Nodes;
	size_t NodesNrOf;
} jobPoolInit_t;

extern void * threadFunction(void * arg);
extern void StartThreads(void ** instance, size_t threadsNrOf, jobPoolInit_t * jobPoolInit);
extern void JoinThreads(void * instance);

#endif /* SRC_NODEEXECUTOR_H_ */
