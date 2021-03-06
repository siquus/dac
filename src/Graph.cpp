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

#include "GlobalDefines.h"
#include "Graph.h"

#include "Module.h"
#include "ControlTransfer.h"
#include "Interface.h"

class Hash {
	// TODO: The following hash function was the first one to be found online.
	// See http://www.cse.yorku.ca/~oz/hash.html
public:
	void Put(const uint8_t * pData, size_t nData)
	{
		for(size_t pos = 0; pos < nData; pos++)
		{
			Hash_ = ((Hash_ << 5) + Hash_) + pData[pos];
		}
	}

	uint32_t Get()
	{
		return Hash_;
	}

private:
	uint32_t Hash_ = 5381;
};

const std::string &Graph::Name() const
{
	return name_;
}

void Graph::Init(const std::string &name)
{
	name_ = name;
}

Graph::Graph(const std::string &name) {
	Init(name);
}

Graph::Graph(const char * name) {
	Init(std::string(name));
}

Graph::~Graph() {
	// TODO Auto-generated destructor stub
}

Node::Id_t Graph::AddNode(Node * node)
{
	// Add the node
	node->id = nextNodeId_;
	nodes_.insert(std::pair<Node::Id_t, Node>{node->id, *node});

	nextNodeId_++;

	return node->id ;
}

bool Graph::AddChild(Node::Id_t parent, Node::Id_t child)
{
	// Search for parent and add child
	auto parentNode = nodes_.find(parent);
	if(nodes_.end() == parentNode)
	{
		Error("Could not find parent!\n");
		return false;
	}

	parentNode->second.ChildrenModifiable()->insert(child);

	return true;
}

const std::map<Node::Id_t, Node> * Graph::GetNodes() const
{
	return &nodes_;
}

const Node * Graph::GetNode(Node::Id_t id) const
{
	auto node = nodes_.find(id);
	if(nodes_.end() == node)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	return &node->second;
}

Node * Graph::GetNodeModifyable(Node::Id_t id)
{
	auto node = nodes_.find(id);
	if(nodes_.end() == node)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	return &node->second;
}

bool Graph::DeleteChildReferences(Node::Id_t child)
{
	for(auto &parentPair: nodes_)
	{
		for(auto childIt = parentPair.second.Children()->begin(); childIt != parentPair.second.Children()->end(); childIt++)
		{
			if(child == *childIt)
			{
				parentPair.second.ChildrenModifiable()->erase(childIt);
				break;
			}
		}
	}

	return true;
}

bool Graph::AddParent(Node::Id_t parent, Node::Id_t child)
{
	// search for Child and add parent
	auto childPair = nodes_.find(child);
	if(nodes_.end() == childPair)
	{
		Error("Could not find node!\n");
		return false;
	}

	childPair->second.ParentsModifiable()->push_back(parent);

	return AddChild(parent, child);
}

const char * Node::getName() const
{
	return getName(Type_);
}

const char* Node::getName(Type type)
{
	switch(type)
	{
	case Type::VECTOR:
		return "VECTOR";

	case Type::VECTOR_ADDITION:
		return "VECTOR_ADDITION";

	case Type::VECTOR_CONTRACTION:
		return "VECTOR_CONTRACTION";

	case Type::VECTOR_SCALAR_PRODUCT:
		return "VECTOR_SCALAR_PRODUCT";

	case Type::VECTOR_VECTOR_PRODUCT:
		return "VECTOR_VECTOR_PRODUCT";

	case Type::VECTOR_POWER:
		return "VECTOR_POWER";

	case Type::VECTOR_COMPARISON_IS_SMALLER:
		return "VECTOR_COMPARISON_IS_SMALLER";

	case Type::VECTOR_KRONECKER_DELTA_PRODUCT:
		return "VECTOR_KRONECKER_DELTA_PRODUCT";

	case Type::VECTOR_PERMUTATION:
		return "VECTOR_PERMUTATION";

	case Type::VECTOR_PROJECTION:
		return "VECTOR_PROJECTION";

	case Type::VECTOR_CROSS_CORRELATION:
		return "VECTOR_CROSS_CORRELATION";

	case Type::VECTOR_MAX_POOL:
		return "VECTOR_MAX_POOL";

	case Type::VECTOR_JOIN_INDICES:
		return "VECTOR_JOIN_INDICES";

	case Type::VECTOR_INDEX_SPLIT_SUM:
		return "VECTOR_INDEX_SPLIT_SUM";

	case Type::OUTPUT:
		return "OUTPUT";

	case Type::INPUT:
		return "INPUT";

	case Type::CONTROL_TRANSFER_WHILE:
		return "CONTROL_TRANSFER_WHILE";

	case Node::Type::NONE:
		Error("Node::Type::None not allowed!!\n");
		return nullptr;

	default:
		Error("Unknown Type %u\n", (uint8_t) type);
		return nullptr;
	}

	// should not be reached
	return nullptr;
}

void Node::UseAsStorageFor(Id_t id)
{
	usedAsStorageBy_.insert(id);
}

bool Node::RemoveStorageFor(Id_t id)
{
	if(usedAsStorageBy_.erase(id))
	{
		return true;
	}

	return false;
}

bool Node::UsedAsStorageByOthers() const
{
	return !usedAsStorageBy_.empty();
}

// Create a hash which will *help* to identify equal nodes.
// Hash will be the same for nodes of the same type
// with the same parents.
// NOTE: This does not mean they perform the same operation!
size_t Node::getPartialHash() const
{
	Hash hash;

	hash.Put((const uint8_t*) parents.data(), parents.size() * sizeof(parents[0]));
	hash.Put((const uint8_t *) &Type_, sizeof(Type_));
	hash.Put((const uint8_t *) &Object_, sizeof(Object_));

	return hash.Get();
}

bool Node::sameObject(const Node &lNode, const Node &rNode)
{
	if(lNode.Object_ != rNode.Object_)
	{
		return false;
	}

	if(
			((nullptr != lNode.ObjectPt_) && (nullptr == rNode.ObjectPt_)) ||
			((nullptr == lNode.ObjectPt_) && (nullptr != rNode.ObjectPt_)))
	{
		return false;
	}
	else if(nullptr == lNode.ObjectPt_) // both are nullptr by above
	{
		return true;
	}

	// Same object with non-nullptr
	switch(lNode.Object_)
	{
	default: // no break intended
	case Object_t::NONE: // then it should have nullptr!
		Error("Error comparing Objects!\n");
		return false;

	case Object_t::MODULE_VECTORSPACE_VECTOR:
		{
			auto lVec = (const Algebra::Module::VectorSpace::Vector *) lNode.ObjectPt_;
			auto rVec = (const Algebra::Module::VectorSpace::Vector *) rNode.ObjectPt_;

			if(!Algebra::Module::VectorSpace::AreEqual(lVec->Space(), rVec->Space()))
			{
				return false;
			}

			if(!lVec->SameValue(lVec, rVec))
			{
				return false;
			}
		}
		break;

	case Object_t::INTERFACE_OUTPUT:
		// TODO: currently, interface objects only contain the name
		// so technically they are the same with different names.
		// with this code we are simply replacing one of them, which might be unexpected.
		break;

	case Object_t::INTERFACE_INPUT:
	{
		auto lIn = (const Interface::Input *) lNode.ObjectPt_;
		auto rIn = (const Interface::Input *) rNode.ObjectPt_;

		if(!lIn->AreEqual(lIn, rIn))
		{
			return false;
		}
	}
		break;
	}

	return true;
}

bool Node::sameType(const Node &lNode, const Node &rNode)
{
	if(lNode.Type_ != rNode.Type_)
	{
		return false;
	}

	if(
			((nullptr != lNode.TypeParameters_) && (nullptr == rNode.TypeParameters_)) ||
			((nullptr == lNode.TypeParameters_) && (nullptr != rNode.TypeParameters_)))
	{
		return false;
	}
	else if(nullptr == lNode.TypeParameters_) // both are nullptr by above
	{
		return true;
	}

	switch(lNode.Type_)
	{
	default: // no break intended
	case Type::VECTOR: // no break intended
	case Type::VECTOR_ADDITION: // no break intended
	case Type::VECTOR_SCALAR_PRODUCT: // no break intended
	case Type::VECTOR_VECTOR_PRODUCT: // no break intended
	case Type::VECTOR_POWER: // no break intended
	case Type::VECTOR_COMPARISON_IS_SMALLER: // no break intended
	case Type::OUTPUT: // no break intended
	case Type::INPUT: // no break intended
	case Type::VECTOR_CROSS_CORRELATION: // no break intended
		Error("Error comparing node types!\n");
		return false;

	case Type::CONTROL_TRANSFER_WHILE:
	{
		auto lWhile = (const ControlTransferParameters_t*) lNode.TypeParameters_;
		auto rWhile = (const ControlTransferParameters_t*) rNode.TypeParameters_;

		if((lWhile->BranchFalse != rWhile->BranchFalse) || (lWhile->BranchTrue != rWhile->BranchTrue))
		{
			return false;
		}
	}
	break;

	case Type::VECTOR_CONTRACTION:
	{
		auto lContr = (const contractParameters_t*) lNode.TypeParameters_;
		auto rContr = (const contractParameters_t*) rNode.TypeParameters_;

		if((!std::equal(lContr->lfactors.begin(), lContr->lfactors.end(), rContr->lfactors.begin())) ||
				(!std::equal(lContr->rfactors.begin(), lContr->rfactors.end(), rContr->rfactors.begin())))
		{
			return false;
		}
	}
	break;

	case Type::VECTOR_KRONECKER_DELTA_PRODUCT:
	{
		auto lKron = (const KroneckerDeltaParameters_t*) lNode.TypeParameters_;
		auto rKron = (const KroneckerDeltaParameters_t*) rNode.TypeParameters_;
		if(lKron->Scaling != rKron->Scaling) // TODO: Really do exact float comparison?
		{
			return false;
		}

		if(!std::equal(lKron->DeltaPair.begin(), lKron->DeltaPair.end(), rKron->DeltaPair.begin()))
		{
			return false;
		}
	}
	break;

	case Type::VECTOR_PERMUTATION:
	{
		auto lPerm = (const permuteParameters_t*) lNode.TypeParameters_;
		auto rPerm = (const permuteParameters_t*) rNode.TypeParameters_;

		if(!std::equal(lPerm->indices.begin(), lPerm->indices.end(), rPerm->indices.begin()))
		{
			return false;
		}
	}
	break;

	case Type::VECTOR_JOIN_INDICES:
	{
		auto lJoin = (const joinIndicesParameters_t*) lNode.TypeParameters_;
		auto rJoin = (const joinIndicesParameters_t*) rNode.TypeParameters_;

		if(!std::equal(lJoin->Indices.begin(), lJoin->Indices.end(), rJoin->Indices.begin()))
		{
			return false;
		}
	}
	break;

	case Type::VECTOR_INDEX_SPLIT_SUM:
	{
		auto lSplit = (const splitSumIndicesParameters_t*) lNode.TypeParameters_;
		auto rSplit= (const splitSumIndicesParameters_t*) rNode.TypeParameters_;

		if(!std::equal(lSplit->SplitPosition.begin(), lSplit->SplitPosition.end(), rSplit->SplitPosition.begin()))
		{
			return false;
		}
	}
	break;

	case Type::VECTOR_MAX_POOL:
	{
		auto lPool = (const PoolParameters_t*) lNode.TypeParameters_;
		auto rPool= (const PoolParameters_t*) rNode.TypeParameters_;

		if(!std::equal(lPool->PoolSize.begin(), lPool->PoolSize.end(), rPool->PoolSize.begin()))
		{
			return false;
		}
	}
	break;

	case Type::VECTOR_PROJECTION:
	{
		auto lProj = (const projectParameters_t*) lNode.TypeParameters_;
		auto rProj = (const projectParameters_t*) rNode.TypeParameters_;

		if(!std::equal(lProj->range.begin(), lProj->range.end(), rProj->range.begin()))
		{
			return false;
		}
	}
	break;
	}

	return true;
}

bool Node::areDuplicate(const Node &lNode, const Node &rNode)
{
	if(!std::equal(lNode.parents.begin(), lNode.parents.end(), rNode.parents.begin()))
	{
		return false;
	}

	if(lNode.storedIn_ != rNode.storedIn_)
	{
		return false;
	}

	if(!std::equal(lNode.usedAsStorageBy_.begin(), lNode.usedAsStorageBy_.end(), rNode.usedAsStorageBy_.begin()))
	{
		return false;
	}

	if(!sameObject(lNode, rNode))
	{
		return false;
	}

	if(!sameType(lNode, rNode))
	{
		return false;
	}

	return true;
}

bool Node::StoreIn(Id_t id)
{
	// TODO: Check compatibility
	storedIn_ = id;

	return true;
}

Node::Node(Object_t object, const void * pObject, Type type, void * pType)
{
	if((Object_t::NONE != object) && (nullptr == pObject))
	{
		Error("Object Pointer can't be null if object is specified!\n");
	}

	Object_ = object;
	ObjectPt_ = pObject;

	Type_ = type;
	TypeParameters_ = pType;
}

Node::Id_t Node::IsStoredIn() const
{
	return storedIn_;
}

Node::Object_t Node::GetObject() const
{
	return Object_;
}

const void * Node::GetObjectPt() const
{
	return ObjectPt_;
}

Node::Type Node::GetType() const
{
	return Type_;
}

const void * Node::TypeParameters() const
{
	return TypeParameters_;
}

void * Node::TypeParametersModifiable()
{
	return TypeParameters_;
}

const std::vector<Node::Id_t> * Node::Parents() const
{
	return &parents;
}

std::vector<Node::Id_t> * Node::ParentsModifiable()
{
	return &parents;
}

const std::set<Node::Id_t> * Node::Children() const
{
	return &children;
}

std::set<Node::Id_t> * Node::ChildrenModifiable()
{
	return &children;
}

bool Graph::GetRootAncestors(std::set<Node::Id_t> * rootParents, Node::Id_t child) const
{
	const Node * childNode = GetNode(child);
	if(nullptr == childNode)
	{
		Error("Could not find child node!\n");
		return false;
	}

	if(0 == childNode->Parents()->size())
	{
		rootParents->insert(childNode->id);
		return true;
	}

	for(const auto &parentId: *childNode->Parents())
	{
		if(false == GetRootAncestors(rootParents, parentId))
		{
			return false;
		}
	}

	return true;
}

void Graph::RemoveDuplicates()
{
	size_t infLoopTimein = nodes_.size();
	size_t nodesRemoved;
	do {
		nodesRemoved = 0; // As long as nodes are removed do this.

		if(0 == infLoopTimein--)
		{
			Error("RemoveDuplicates() did not finish!\n");
			break;
		}

		// Search for all Nodes of same type with same parents
		// Get all nodes with equal partial hash
		std::map<size_t, std::vector<Node::Id_t>> hashMap;
		for(const auto &nodePair: nodes_)
		{
			const auto insertRet = hashMap.insert(std::pair<size_t, std::vector<Node::Id_t>>{
				nodePair.second.getPartialHash(),
						std::vector<Node::Id_t>{nodePair.first}});

			if(!insertRet.second) // hash already present.
			{
				insertRet.first->second.push_back(nodePair.first);
			}
		}

		for(auto &hashPair: hashMap)
		{
			if(1 >= hashPair.second.size())
			{
				continue; // no duplicate!
			}

			printf("Found %lu possible duplicates Nodes: ", hashPair.second.size());

			std::vector<std::vector<Node::Id_t>> duplicates;
			for(auto currNodeIt = hashPair.second.begin(); currNodeIt < hashPair.second.end() - 1; currNodeIt++)
			{
				std::vector<Node::Id_t> currentDup{*currNodeIt};

				const auto hashNode = nodes_.find(*currNodeIt);

				for(auto cmpNodeIt = currNodeIt + 1; cmpNodeIt < hashPair.second.end(); cmpNodeIt++)
				{
					const auto cmpNode = nodes_.find(*cmpNodeIt);

					if(Node::areDuplicate(hashNode->second, cmpNode->second))
					{
						currentDup.push_back(*cmpNodeIt);
						cmpNodeIt = hashPair.second.erase(cmpNodeIt);
					}
				}

				duplicates.push_back(currentDup);
			}

			printf("Reduced to %lu Nodes\n", duplicates.size());

			for(const auto &equalNodes: duplicates)
			{
				if(1 >= equalNodes.size())
				{
					continue; // nothing to do.
				}

				nodesRemoved += equalNodes.size() - 1;

				ReduceToOne(equalNodes);
			}
		}

	} while(nodesRemoved);

	return;
}

bool Graph::ReduceToOne(const std::vector<Node::Id_t> &nodes)
{
	if(1 >= nodes.size())
	{
		return true; // nothing to do
	}

	printf("Replacing ");
	for(size_t nodePos = 1; nodePos < nodes.size(); nodePos++)
	{
		printf("%u, ", nodes[nodePos]);
	}
	printf("with %u\n", nodes[0]);

	const Node::Id_t newNodeId = nodes[0];
	auto newNodeIt = nodes_.find(newNodeId);

	// Remove nodes (we'll keep the first one and replace the others with it)
	for(size_t nodePos = 1; nodePos < nodes.size(); nodePos++)
	{
		// Carry over children
		auto eraseNodeIt = nodes_.find(nodes[nodePos]);
		newNodeIt->second.ChildrenModifiable()->insert(
				eraseNodeIt->second.Children()->begin(),
				eraseNodeIt->second.Children()->end());

		nodes_.erase(eraseNodeIt);
	}

	// Go through all nodes and replace occurrences of node ids from nodes with newNodeId:
	for(auto &nodePair: nodes_)
	{
		// Go through all class Node::Id_t objects and replace instances of nodes with newNodeId
		for(size_t nodePos = 1; nodePos < nodes.size(); nodePos++)
		{
			const Node::Id_t cmpId = nodes[nodePos];

			for(Node::Id_t &parent: *nodePair.second.ParentsModifiable())
			{
				if(cmpId == parent)
				{
					parent = newNodeId;
				}
			}

			if(nodePair.second.ChildrenModifiable()->erase(cmpId))
			{
				nodePair.second.ChildrenModifiable()->insert(newNodeId);
			}

			if(Node::Type::CONTROL_TRANSFER_WHILE == nodePair.second.GetType())
			{
				auto pWhile = (Node::ControlTransferParameters_t*) nodePair.second.TypeParametersModifiable();
				if(cmpId == pWhile->BranchTrue)
				{
					pWhile->BranchTrue = newNodeId;
				}

				if(cmpId == pWhile->BranchFalse)
				{
					pWhile->BranchFalse = newNodeId;
				}
			}

			if(cmpId == nodePair.second.IsStoredIn())
			{
				nodePair.second.StoreIn(newNodeId);
			}

			if(nodePair.second.RemoveStorageFor(cmpId))
			{
				nodePair.second.UseAsStorageFor(newNodeId);
			}
		}
	}

	return true;
}

void NodeRef::SetNodeRef(Graph* graph, Node::Id_t id)
{
	graph_ = graph;
	nodeId_ = id;
}

bool NodeRef::StoreIn(const NodeRef* nodeRef) const
{
	if(nodeRef->graph_ != graph_)
	{
		Error("Different Graphs!\n");
		return false;
	}

	// TODO: Test that nodes are compatible!
	Node * thisNode = graph_->GetNodeModifyable(nodeId_);
	if(nullptr == thisNode)
	{
		Error("Could not find node!\n");
		return false;
	}

	thisNode->StoreIn(nodeRef->nodeId_);

	Node * storageNode = graph_->GetNodeModifyable(nodeRef->nodeId_);
	if(nullptr == storageNode)
	{
		Error("Could not find node!\n");
		return false;
	}

	storageNode->UseAsStorageFor(nodeId_);

	return true;
}

void NodeRef::PushParent(Node::Id_t parent)
{
	bool success = graph_->AddParent(parent, nodeId_);

	if(!success)
	{
		Error("Could not push parent!\n");
	}
}

Node::Id_t NodeRef::Id() const
{
	return nodeId_;
}

Graph* NodeRef::GetGraph() const
{
	return graph_;
}
