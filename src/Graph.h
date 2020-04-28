/*
 * Graph.h
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#ifndef SRC_GRAPH_H_
#define SRC_GRAPH_H_

#include <vector>
#include <map>
#include <set>
#include <stdint.h>
#include <string>

class Node {
public:
	enum class Type {
		NONE, // Value not allowed!
		VECTOR,
		VECTOR_ADDITION,
		VECTOR_CONTRACTION,
		VECTOR_SCALAR_PRODUCT, // a.k.a. scalar product, e.g. A_ij = B_ij * C_k, |k| = 1
		VECTOR_VECTOR_PRODUCT, // a.k.a. tensor product, e.g. A_ijk = B_ij * C_k
		VECTOR_POWER,
		VECTOR_COMPARISON_IS_SMALLER,
		VECTOR_KRONECKER_DELTA_PRODUCT, // i.e. deta^i_j * delta^k_l * ...
		VECTOR_PERMUTATION,
		VECTOR_JOIN_INDICES,
		VECTOR_PROJECTION,
		OUTPUT,
		CONTROL_TRANSFER_WHILE,
	};

	typedef struct {
		std::vector<uint32_t> lfactors;
		std::vector<uint32_t> rfactors;
	} contractParameters_t;

	typedef struct {
		std::vector<uint32_t> indices;
	} permuteParameters_t;

	typedef struct {
		std::vector<std::pair<uint32_t, uint32_t>> range;
	} projectParameters_t;

	typedef struct {
		std::vector<std::vector<uint32_t>> Indices; // sorted, small to large
	} joinIndicesParameters_t;

	typedef struct {
		std::vector<uint32_t> DeltaPair; // I.e. for delta^i_j * delta^k_l * , ... position i will contain j and vice versa.
		float Scaling = 1.;
	} KroneckerDeltaParameters_t;

	// TODO: Should be called getTypeStr or so
	static const char* getName(Type type);
	const char * getName() const;

	size_t getPartialHash() const;

	static bool areDuplicate(const Node &lNode, const Node &rNode);
	static bool sameObject(const Node &lNode, const Node &rNode);
	static bool sameTypeParameters(const Node &lNode, const Node &rNode);

	enum class ObjectType {
		NONE,
		MODULE_VECTORSPACE_VECTOR,
		INTERFACE_OUTPUT
	};

	typedef uint32_t Id_t;
	static constexpr Id_t ID_NONE = 0;

	std::vector<Id_t> parents; // TODO: No std::set, because position matters
	std::set<Id_t> children;
	Id_t branchTrue = ID_NONE;
	Id_t branchFalse = ID_NONE;

	Type type = Type::NONE;
	const void* typeParameters = nullptr; // see fooParameters_t
	ObjectType objectType = ObjectType::NONE;
	const void* object = nullptr;
	Id_t id = ID_NONE;
	Id_t storedIn_ = ID_NONE;
	bool noStorage_ = false;
	std::set<Id_t> usedAsStorageBy_;
};

class Graph {
public:
	Graph(const std::string &name);
	Graph(const char * name);

	virtual ~Graph();

	Node::Id_t AddNode(Node * node);
	bool AddParent(Node::Id_t parent, Node::Id_t child);
	const std::map<Node::Id_t, Node> * GetNodes() const;
	const Node * GetNode(Node::Id_t id) const;
	Node * GetNodeModifyable(Node::Id_t id);
	bool DeleteChildReferences(Node::Id_t child);
	const std::string &Name() const;

	bool ReduceToOne(const std::vector<Node::Id_t> &nodes);

	bool GetRootAncestors(std::set<Node::Id_t> * rootParents, Node::Id_t child) const;

	void RemoveDuplicates();

private:
	std::map<Node::Id_t, Node> nodes_;
	std::string name_;

	Node::Id_t nextNodeId_ = Node::ID_NONE + 1;

	void Init(const std::string &name);
	bool AddChild(Node::Id_t parent, Node::Id_t child);
};

class NodeRef {
public:
	bool StoreIn(const NodeRef* nodeRef) const;

	Node::Id_t nodeId_ = Node::ID_NONE;
	Graph* graph_ = nullptr;
};

#endif /* SRC_GRAPH_H_ */
