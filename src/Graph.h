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

class Node {
public:
	enum class Type {
		VECTOR,
		VECTOR_ADDITION,
		VECTOR_CONTRACTION,
		VECTOR_SCALAR_MULTIPLICATION,
		VECTOR_COMPARISON_IS_SMALLER,
		OUTPUT,
		CONTROL_TRANSFER_WHILE,
	};

	static const char* getName(Type type);

	enum class ObjectType {
		NONE,
		MODULE_VECTORSPACE_VECTOR,
		INTERFACE_OUTPUT,
		CONTROL_TRANSFER_WHILE,
	};

	typedef uint32_t Id_t;
	static constexpr Id_t ID_NONE = 0;

	typedef enum {
		HARDWARE_TYPE_NONE,
		HARDWARE_TYPE_CPU,
	} hardwareType_t;

	typedef uint16_t HardwareIdentifier_t;

	typedef struct {
		hardwareType_t type = HARDWARE_TYPE_NONE;
		HardwareIdentifier_t identifier = UINT16_MAX;
	} Hardware_t;

	void* object = nullptr;
	std::vector<Id_t> parents;
	std::vector<Id_t> children;
	Type type;
	ObjectType objectType;
	Id_t id;
	Hardware_t hardware;
	Id_t storedIn_ = ID_NONE;
	std::vector<Id_t> usedAsStorageBy_;
};

class Graph {
public:
	Graph();
	virtual ~Graph();

	Node::Id_t nextNodeId_ = Node::ID_NONE + 1;

	Node::Id_t AddNode(Node * node);
	bool AddParent(Node::Id_t parent, Node::Id_t child);
	const std::vector<Node> * GetNodes() const;
	const Node * GetNode(Node::Id_t id) const;
	std::vector<Node> * GetNodesModifyable();
	bool DeleteChildReferences(Node::Id_t child);

private:
	std::vector<Node> nodes_;

	bool AddChild(Node::Id_t parent, Node::Id_t child);
};

class NodeRef {
public:
	Node::Id_t nodeId_ = Node::ID_NONE;
	Graph* graph_ = nullptr;

	bool StoreIn(const NodeRef* nodeRef);
};

#endif /* SRC_GRAPH_H_ */
