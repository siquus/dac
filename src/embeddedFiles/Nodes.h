/*
 * Nodes.h
 *
 *  Created on: Sep 6, 2019
 *      Author: derommo
 */

#ifndef SRC_EMBEDDEDFILES_NODES_H_
#define SRC_EMBEDDEDFILES_NODES_H_

#define NODE_T_MAX_EDGE_NUMBER 42u

typedef void (*instruction_t)(void);

typedef struct node_s {
	instruction_t instruction;
	const struct node_s * parents[NODE_T_MAX_EDGE_NUMBER];
	struct node_s * children[NODE_T_MAX_EDGE_NUMBER];
	uint32_t exeCnt;
	const uint16_t parentsNrOf;
	const uint16_t childrenNrOf;
	const uint16_t id;
} node_t;

extern node_t nodes[];

#endif /* SRC_EMBEDDEDFILES_NODES_H_ */
