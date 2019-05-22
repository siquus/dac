/*
 * Graph.h
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#ifndef SRC_GRAPH_H_
#define SRC_GRAPH_H_

#include <vector>
#include <stdint.h>

class Graph {
public:
	Graph();
	virtual ~Graph();

	enum class NodeType {
		VECTOR,
		VECTOR_ADDITION,
		VECTOR_SCALAR_MULTIPLICATION,
		OUTPUT,
	};

	enum class ObjectType {
		MODULE_VECTORSPACE_VECTOR,
		INTERFACE_OUTPUT,
	};

	typedef uint32_t NodeId_t;
	static const NodeId_t NODE_ID_NONE;
	NodeId_t nextNodeId_ = NODE_ID_NONE + 1;

	typedef struct Node {
		void* object = nullptr;
		std::vector<NodeId_t> parents;
		std::vector<NodeId_t> children;
		NodeType nodeType;
		ObjectType objectType;
		NodeId_t id;
	} Node_t;

	NodeId_t AddNode(Node_t * node);
	bool AddParent(NodeId_t parent, NodeId_t child);

private:
	std::vector<Node_t> nodes_;

	bool AddChild(NodeId_t parent, NodeId_t child);
};

#endif /* SRC_GRAPH_H_ */
