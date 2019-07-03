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

	typedef enum {
		HARDWARE_TYPE_NONE,
		HARDWARE_TYPE_CPU,
	} hardwareType_t;

	typedef uint16_t HardwareIdentifier_t;

	typedef struct {
		hardwareType_t type = HARDWARE_TYPE_NONE;
		HardwareIdentifier_t identifier = UINT16_MAX;
	} Hardware_t;

	typedef struct Node {
		void* object = nullptr;
		std::vector<NodeId_t> parents;
		std::vector<NodeId_t> children;
		NodeType nodeType;
		ObjectType objectType;
		NodeId_t id;
		Hardware_t hardware;
	} Node_t;

	NodeId_t AddNode(Node_t * node);
	bool AddParent(NodeId_t parent, NodeId_t child);
	const std::vector<Node_t> * GetNodes() const;
	std::vector<Node_t> * GetNodesModifyable();

private:
	std::vector<Node_t> nodes_;

	bool AddChild(NodeId_t parent, NodeId_t child);
};

#endif /* SRC_GRAPH_H_ */
