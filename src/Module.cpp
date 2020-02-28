/*
 * Module.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#include <stdlib.h>
#include <cstring>
#include <type_traits>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "Module.h"

#include "GlobalDefines.h"

using namespace Algebra;
using namespace Module;

template VectorSpace::Vector * VectorSpace::Element<float>(Graph * graph, const std::vector<float> &initializer) const;

template<typename T>
static bool hasDublicates(const std::vector<T> &vec)
{
	std::set<T> valueSet;

	for(const T &value: vec)
	{
		auto result = valueSet.insert(value);
		if(false == result.second) // element is not new
		{
			return true;
		}
	}

	return false;
}

VectorSpace::VectorSpace(Ring::type_t ring, dimension_t dim)
{
	factors_.push_back(simpleVs_t{ring, dim});
}

dimension_t VectorSpace::GetDim() const
{
	dimension_t retDim = 1;
	for(const simpleVs_t &factor: factors_)
	{
		retDim *= factor.dim_;
	}

	return retDim;
}

Ring::type_t VectorSpace::GetRing() const
{
	Ring::type_t retRing = Ring::None;
	for(const simpleVs_t &factor: factors_)
	{
		retRing = Ring::GetSuperiorRing(retRing, factor.ring_);
	}

	return retRing;
}

void VectorSpace::GetStrides(std::vector<uint32_t> * strides) const
{
	strides->resize(factors_.size());
	for(int fac = 0; fac < factors_.size(); fac++)
	{
		uint32_t stride = 1;
		for(int prodFac = fac + 1; prodFac < factors_.size(); prodFac++)
		{
			stride *= factors_[prodFac].dim_;
		}

		strides->at(fac) = stride;
	}
}

template<typename inType>
VectorSpace::Vector * VectorSpace::Element(Graph * graph, const std::vector<inType>  &initializer) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	if(initializer.size() != GetDim())
	{
		Error("Initializer dimensions do not match (%lu vs %i)!\n",
				initializer.size(), GetDim());
		return nullptr;
	}

	if(!Ring::IsCompatible(GetRing(), initializer))
	{
		Error("Initializer for Vector of incompatible Type!\n");
		return nullptr;
	}

	Vector * retVec = new Vector;
	if(nullptr == retVec)
	{
		Error("Could not malloc Vec\n");
		return nullptr;
	}

	Node node;
	node.type = Node::Type::VECTOR;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	Node::Id_t nodeId = graph->AddNode(&node);
	if(Node::ID_NONE == nodeId)
	{
		Error("Could not add node!\n");
		return nullptr;
	}

	retVec->__value_ = initializer.data();
	retVec->__space_ = this;
	retVec->graph_ = graph;
	retVec->nodeId_ = nodeId;

	return retVec;
}

VectorSpace::Vector * VectorSpace::Element(Graph* graph, const KroneckerDeltaParameters_t &initializer) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	if(initializer.DeltaPair.size() != factors_.size())
	{
		Error("Initializer dimensions do not match (%lu vs %i)!\n",
				initializer.DeltaPair.size(), factors_.size());
		return nullptr;
	}

	Vector * retVec = new Vector;
	if(nullptr == retVec)
	{
		Error("Could not malloc Vec\n");
		return nullptr;
	}

	Node node;
	node.type = Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;

	KroneckerDeltaParameters_t * param = new KroneckerDeltaParameters_t;
	*param = initializer;

	node.typeParameters = param;
	node.object = retVec;

	Node::Id_t nodeId = graph->AddNode(&node);
	if(Node::ID_NONE == nodeId)
	{
		Error("Could not add node!\n");
		return nullptr;
	}

	retVec->__space_ = this;
	retVec->graph_ = graph;
	retVec->nodeId_ = nodeId;

	return retVec;
}

VectorSpace::Vector* VectorSpace::Vector::Multiply(const Vector* vec)
{
	if(1 != vec->__space_->GetDim())
	{
		Error("Dimension Mismatch!\n");
		return nullptr;
	}

	if(graph_ != vec->graph_)
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	// Infer space
	Ring::type_t inferredRing = Ring::GetSuperiorRing(__space_->GetRing(), vec->__space_->GetRing());
	if(Ring::None == inferredRing)
	{
		Error("Incompatible Rings\n");
		return nullptr;
	}

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;

	VectorSpace * retSpace = nullptr;
	retSpace = new VectorSpace(inferredRing, __space_->GetDim());

	if(nullptr == retSpace)
	{
		Error("Could not create VectorSpace");
		return nullptr;
	}

	retVec->__space_ = retSpace;

	Node node;
	node.parents.push_back(nodeId_);
	node.parents.push_back(vec->nodeId_);
	node.type = Node::Type::VECTOR_SCALAR_MULTIPLICATION;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	retVec->nodeId_ = graph_->AddNode(&node);

	if(Node::ID_NONE == retVec->nodeId_)
	{
		Error("Could not add Node!\n");
		return nullptr;
	}

	return retVec;
}

VectorSpace::Vector* VectorSpace::Vector::IsSmaller(const Vector* vec)
{
	if(__space_->GetDim() != vec->__space_->GetDim())
	{
		Error("Dimension Mismatch!\n");
		return nullptr;
	}

	if(graph_ != vec->graph_)
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;
	retVec->__space_ = new VectorSpace(Ring::Int32, 1);

	Node node;
	node.parents.push_back(nodeId_);
	node.parents.push_back(vec->nodeId_);
	node.type = Node::Type::VECTOR_COMPARISON_IS_SMALLER;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	retVec->nodeId_ = graph_->AddNode(&node);

	if(Node::ID_NONE == retVec->nodeId_)
	{
		Error("Could not add Node!\n");
		return nullptr;
	}

	return retVec;
}

bool VectorSpace::Vector::AreCompatible(const Vector* vec1, const Vector* vec2)
{
	if(vec1->graph_ != vec2->graph_)
	{
		Error("Not on the same Graph!\n");
		return false;
	}

	if(vec1->__space_->factors_.size() != vec2->__space_->factors_.size())
	{
		Error("Product Space has different number of factors!\n");
		return false;
	}

	for(int factor = 0; factor < vec1->__space_->factors_.size(); factor++)
	{
		if(vec1->__space_->factors_[factor].dim_ != vec2->__space_->factors_[factor].dim_)
		{
			Error("Factor %u is of different dimension!\n", factor);
			return false;
		}

		if(vec1->__space_->factors_[factor].ring_ != vec2->__space_->factors_[factor].ring_)
		{
			Error("Factor %u has a different ring!\n", factor);
			return false;
		}
	}

	return true;
}

VectorSpace::Vector * VectorSpace::Vector::Derivative(const Vector* vec)
{
	// TODO: Carry out this code in generation phase.
	if(graph_ != vec->graph_)
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	// Find all operations where this node was effected by the input
	// Find this node
	const Node* thisNode = graph_->GetNode(nodeId_);
	if(nullptr == thisNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	// Go through all parents and their parents ... to see if input is involved
	// and build a graph for this
	std::map<Node::Id_t, depNode_t> dependenceGraph;
	TraverseParents(&dependenceGraph, nodeId_, vec->nodeId_);

	// Now we have a graph who's roots are either the dependency node or another node (dah!).
	// What's special is that there will be no non-root dep. node!
	// So: Iteratively remove all roots which are not the dependency node.
	std::vector<Node::Id_t> depNodesToRemove;
	do
	{
		for(const Node::Id_t idToErase: depNodesToRemove)
		{
			for(const Node::Id_t &childId: dependenceGraph[idToErase].children)
			{
				dependenceGraph[childId].parents.erase(idToErase);
			}

			dependenceGraph.erase(idToErase);
		}

		depNodesToRemove.clear();

		for(auto &dep: dependenceGraph)
		{
			if((0 == dep.second.parents.size()) && (vec->nodeId_ != dep.first))
			{
				depNodesToRemove.push_back(dep.first);
			}
		}
	} while(depNodesToRemove.size());

	// The single root is the dependency node now.
#define PRINT_FINAL_DEP_TREE 1
#if PRINT_FINAL_DEP_TREE
	printf("dNode Id %u, Dep Node Id %u\n", nodeId_, vec->nodeId_);
	for(const auto &dep: dependenceGraph)
	{
		printf("Node Id%u: Parents: ", dep.first);
		for(const auto &parent: dep.second.parents)
		{
			printf("%u, ", parent);
		}

		printf("Children : ");
		for(const auto &child: dep.second.children)
		{
			printf("%u, ", child);
		}
		printf("\n");
	}
#endif // PRINT_FINAL_DEP_TREE

	// Chain rule: Contract / Add the derivatives of all nodes above
	// Start Node: Sum over all derivatives w.r.t. all parents

	return CreateDerivative(&dependenceGraph, this, vec->nodeId_);
}

void VectorSpace::Vector::TraverseParents(std::map<Node::Id_t, depNode_t> * depNodes, Node::Id_t currentNode, Node::Id_t depNodeId) const
{
	const Node * currentNodePt = graph_->GetNode(currentNode);
	if(nullptr == currentNodePt)
	{
		Error("Could not find node Id%u!\n", currentNode);
		return;
	}

	for(const auto &nextNodeId: currentNodePt->parents)
	{
		depNode_t &nextDepNode = (*depNodes)[nextNodeId];

		nextDepNode.children.insert(currentNode);
		(*depNodes)[currentNode].parents.insert(nextNodeId);

		if(nextNodeId != depNodeId)
		{
			TraverseParents(depNodes, nextNodeId, depNodeId);
		}
	}
}

VectorSpace::Vector* VectorSpace::Vector::CreateDerivative(std::map<Node::Id_t, depNode_t> * depNodes, const VectorSpace::Vector * currentVec, Node::Id_t depNodeId)
{
	if(currentVec->nodeId_ == depNodeId)
	{
		Error("Taking derivative w.r.t. oneself!\n");
		return nullptr;
	}

	if(0 == (*depNodes)[currentVec->nodeId_].parents.size())
	{
		Error("Taking derivative of parentless vector!\n");
		return nullptr;
	}

	// Example:
	// dA_ij(B(C(E), F(E)), D(E)) / dE_kl =
	// (dA_ij / dB_mn)((dB_mn / dC_op)(dC_op / dE_kl) + (dB_mn / dF) / (dF / dE_kl) + (dA_ij / dD_qrs) (dD_qrs / dE_kl)
	// B and D are A's parents. The following for-loop will create the "+" sign above, i.e. sum over all parents
	// Inside that Loop, this function will be called recursively to create the sums for the chain-rule, e.g. above
	// for B(C, F) ... i.e. dC and dF are summed.
	Vector * summandVec = nullptr;
	for(const Node::Id_t &parentId: (*depNodes)[currentVec->nodeId_].parents)
	{
		const Node * parentNode = graph_->GetNode(parentId);
		if(nullptr == parentNode)
		{
			Error("Unknown Node Id!\n");
			return nullptr;
		}

		if(Node::ObjectType::MODULE_VECTORSPACE_VECTOR != parentNode->objectType)
		{
			Error("Can't take derivative w.r.t. non-vector node of object type %u!\n",
					(uint8_t) parentNode->objectType);
			return nullptr;
		}

		// TODO: Check for Null returns
		const Vector * parentVec = (const Vector *) parentNode->object;
		Vector * derivativeVec = CreateDerivative(currentVec, parentVec);

		if(depNodeId != parentId)
		{
			// Call this function recursively if parent is not the variable w.r.t. which
			// the derivative is calculated
			Vector * currentVecChainDerivative = CreateDerivative(depNodes, parentVec, depNodeId);

			std::vector<uint32_t> lfactors(parentVec->__space_->factors_.size());
			std::iota(lfactors.begin(), lfactors.end(), 0);

			std::vector<uint32_t> rfactors(parentVec->__space_->factors_.size());
			std::iota(rfactors.begin(), rfactors.end(),
					currentVecChainDerivative->__space_->factors_.size() - lfactors.size());

			derivativeVec = derivativeVec->Contract(
					currentVecChainDerivative,
					lfactors, rfactors);
		}

		if(nullptr == summandVec) // first run
		{
			summandVec = derivativeVec;
		}
		else
		{
			summandVec = summandVec->Add(derivativeVec);
		}

		if(nullptr == summandVec)
		{
			Error("Could not create derivative!\n");
			return nullptr;
		}
	}

	return summandVec;
}

// For a function V_0 x V_1 .. x V_n -> V, this function creates the derivative,
// i.e. it returns a vector in the space V_arg \tensor V
VectorSpace::Vector* VectorSpace::Vector::CreateDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	// Check if arg actually is a parent to this function
	const Node* fctNode = vecValuedFct->graph_->GetNode(vecValuedFct->nodeId_);
	if(fctNode->parents.end() == std::find(fctNode->parents.begin(), fctNode->parents.end(), arg->nodeId_))
	{
		Error("Tried to take derivative w.r.t. non-existing input node!\n");
		return nullptr;
	}

	switch(fctNode->type)
	{
	case Node::Type::VECTOR_ADDITION:
		return AddDerivative(vecValuedFct, arg);

	case Node::Type::VECTOR_CONTRACTION: // no break intended
	case Node::Type::VECTOR_SCALAR_MULTIPLICATION: // no break intended
		Error("Node Type %s not yet supported taking its derivative!\n", Node::getName(fctNode->type));
		return nullptr;

	default:
		Error("Node Type %s does not support taking its derivative!\n", Node::getName(fctNode->type));
		return nullptr;
	}


	return nullptr;
}

VectorSpace::Vector* VectorSpace::Vector::Contract(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors)
{
	if(graph_ != vec->graph_)
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	if(lfactors.size() != rfactors.size())
	{
		Error("Contraction indice-vectors of different size!\n");
		return nullptr;
	}

	// Check if indices are inside allowed range
	for(size_t index = 0; index < lfactors.size(); index++)
	{
		if((__space_->factors_.size() <= lfactors[index]) || (vec->__space_->factors_.size() <= rfactors[index]))
		{
			Error("At least one given contraction factor is larger than number of factors!\n");
			return nullptr;
		}

		if(__space_->factors_[lfactors[index]].dim_ != vec->__space_->factors_[rfactors[index]].dim_)
		{
			Error("At least one contraction index-pair has different dimension!\n");
			return nullptr;
		}
	}

	// Check for dublicate indices
	if(hasDublicates(lfactors) || hasDublicates(rfactors))
	{
		Error("Dublicate contraction factors!\n");
		return nullptr;
	}

	// Create contracted vector space
	// Create vectors with residual factors after contraction
	// Erase factors starting from the vector's tail
	std::vector<uint32_t> lfactorsSorted = lfactors;
	std::sort(lfactorsSorted.begin(), lfactorsSorted.end());

	std::vector<simpleVs_t> lResidualFactors = __space_->factors_;
	for(int factor = lfactorsSorted.size() - 1; factor >= 0; factor--)
	{
		lResidualFactors.erase(lResidualFactors.begin() + lfactorsSorted[factor]);
	}

	// Erase factors starting from the vector's tail
	std::vector<uint32_t> rfactorsSorted = rfactors;
	std::sort(rfactorsSorted.begin(), rfactorsSorted.end());

	std::vector<simpleVs_t> rResidualFactors = vec->__space_->factors_;
	for(int factor = rfactorsSorted.size() - 1; factor >= 0; factor--)
	{
		rResidualFactors.erase(rResidualFactors.begin() + rfactorsSorted[factor]);
	}


	std::vector<simpleVs_t> factorsVec = lResidualFactors;
	factorsVec.insert(factorsVec.end(), rResidualFactors.begin(), rResidualFactors.end());

	// Special case: Scalar product, i.e. total contraction
	if(0 == factorsVec.size())
	{
		// TODO: Should get superior ring across all factors. Then again: How could they not all be the same?
		Ring::type_t superiorRing = Ring::GetSuperiorRing(
				__space_->factors_[0].ring_,
				vec->__space_->factors_[0].ring_);

		factorsVec.push_back(simpleVs_t{superiorRing, 1});
	}

	VectorSpace * retSpace = nullptr;
	retSpace = new VectorSpace(&factorsVec);
	if(nullptr == retSpace)
	{
		Error("Could not create VectorSpace");
		return nullptr;
	}

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;
	retVec->__space_ = retSpace;

	const Node * thisNode = graph_->GetNode(nodeId_);
	const Node * vecNode = graph_->GetNode(vec->nodeId_);

	Node node;
	if((Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT == thisNode->type) &&
			(Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT == thisNode->type))
	{
		// Example:
		// Say we have C_jklmnp = A_ijkl B_mnip = d_ij d_kl d_mi d_np
		//                           = |i| * d_mj d_dkl d_np, |i| = dim of index i
		// i.e. Kon. Params: {1, 0, 3, 2} {2, 3, 0, 1}
		// Combine the lists to d := {1, 0, 3, 2, 6, 7, 4, 5} i.e. += 4 to all B indices.
		// Perform the sum over index position 0 (pos. of i in A) and 2 + 4 (pos. of i in B, + 4 for pos in d):
		// 1. tmp = d[d[0]];
		//    d[d[0]] = d[d[2 + 4]]; d[d[2 + 4]] = tmp; i.e. d_ij d_mi = |i| d_jm
		// 2. Erase array position 0 and -= 1 all array values > 0 (all of them, the one == 0 was removed in 1.)
		// 3. Erase array position 2 + 4 and -= 1 all array indices < 2 + 4 (again, == 2 + 4 was removed in 1.)
		// 4. C *= |i|
		// done.

		const KroneckerDeltaParameters_t * thisKronParam = (const KroneckerDeltaParameters_t *) thisNode->typeParameters;
		const KroneckerDeltaParameters_t * vecKronParam = (const KroneckerDeltaParameters_t *) vecNode->typeParameters;

		KroneckerDeltaParameters_t * opKronParam = new KroneckerDeltaParameters_t;
		opKronParam->Scaling = thisKronParam->Scaling * vecKronParam->Scaling;

		opKronParam->DeltaPair = thisKronParam->DeltaPair;

		// First just combine the list of delta pairs
		opKronParam->DeltaPair.insert(
				opKronParam->DeltaPair.begin(),
				vecKronParam->DeltaPair.begin(), vecKronParam->DeltaPair.end());

		// Make it valid by increasing the the vecKron indices by size of thisKron indices
		size_t vecKronOffset = thisKronParam->DeltaPair.size();
		for(size_t dPair = vecKronOffset; dPair < opKronParam->DeltaPair.size(); dPair++)
		{
			opKronParam->DeltaPair[dPair] += vecKronOffset;
		}

		// Contract the DeltaPairs
		std::vector<uint32_t> lfactorsNew = lfactors;
		std::vector<uint32_t> rfactorsNew = rfactors;
		for(uint32_t &rfactor: rfactorsNew)
		{
			rfactor += vecKronOffset;
		}

		for(size_t contrFactor = 0; contrFactor < lfactorsNew.size(); contrFactor++)
		{
			const uint32_t lfactor = lfactorsNew[contrFactor];
			const uint32_t rfactor = rfactorsNew[contrFactor];

			uint32_t tmp = opKronParam->DeltaPair[opKronParam->DeltaPair[lfactor]];
			opKronParam->DeltaPair[opKronParam->DeltaPair[lfactor]] = opKronParam->DeltaPair[opKronParam->DeltaPair[rfactor]];
			opKronParam->DeltaPair[opKronParam->DeltaPair[rfactor]] = tmp;

			opKronParam->DeltaPair.erase(opKronParam->DeltaPair.begin() + lfactor);
			opKronParam->DeltaPair.erase(opKronParam->DeltaPair.begin() + rfactor - 1);

			for(uint32_t &dPair: opKronParam->DeltaPair)
			{
				if(rfactor < dPair)
				{
					dPair -= 2;
				}
				else if(lfactor < dPair)
				{
					dPair -= 1;
				}
			}

			for(uint32_t &lfactorNew: lfactorsNew)
			{
				if(lfactor < lfactorNew)
				{
					lfactorNew -= 1;
				}
			}

			for(uint32_t &rfactorNew: rfactorsNew)
			{
				if(rfactor < rfactorNew)
				{
					rfactorNew -= 1;
				}
			}


			opKronParam->Scaling *= __space_->factors_[lfactors[contrFactor]].dim_;
		}

		node.type = Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT;
		node.typeParameters = opKronParam;
	}
	else
	{
		contractValue_t * opParameters = new contractValue_t;
		opParameters->lfactors = lfactors;
		opParameters->rfactors = rfactors;

		node.parents.push_back(nodeId_);
		node.parents.push_back(vec->nodeId_);
		node.type = Node::Type::VECTOR_CONTRACTION;
		node.typeParameters = opParameters;
	}

	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	retVec->nodeId_ = graph_->AddNode(&node);

	if(Node::ID_NONE == retVec->nodeId_)
	{
		Error("Could not add Node!\n");
		return nullptr;
	}

	return retVec;
}

VectorSpace::Vector* VectorSpace::Vector::Contract(const Vector* vec, uint32_t lfactor, uint32_t rfactor)
{
	std::vector<uint32_t> lfactors{lfactor};
	std::vector<uint32_t> rfactors{rfactor};

	return Contract(vec, lfactors, rfactors);
}

VectorSpace::Vector* VectorSpace::Vector::AddDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	Vector* retVec = new Vector;
	retVec->graph_ = vecValuedFct->graph_;

	// The new vector will be of tensor product vector space type.
	// The derivative vector's VS will come first (as in differential forms)
	std::vector<simpleVs_t> factors;
	factors.insert(factors.begin(), arg->__space_->factors_.begin(), arg->__space_->factors_.end());
	factors.insert(factors.end(), vecValuedFct->__space_->factors_.begin(), vecValuedFct->__space_->factors_.end());

	if(arg->__space_->factors_.size() != vecValuedFct->__space_->factors_.size())
	{
		Error("For addition the resulting tensor should have same dimensions as arguments!\n");
		return nullptr;
	}

	retVec->__space_ = new VectorSpace(&factors);

	// Derivative of Add is easy: Just the product of Kronecker Deltas
	Node node;
	node.type = Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT;
	KroneckerDeltaParameters_t * opParameters = new KroneckerDeltaParameters_t;
	opParameters->DeltaPair.resize(factors.size());
	for(size_t factor = 0; factor < opParameters->DeltaPair.size() / 2; factor++)
	{
		opParameters->DeltaPair[factor] = opParameters->DeltaPair.size() / 2 + factor ;
	}

	for(size_t factor = opParameters->DeltaPair.size() / 2; factor < opParameters->DeltaPair.size(); factor++)
	{
		opParameters->DeltaPair[factor] = factor - opParameters->DeltaPair.size() / 2;
	}

	node.typeParameters = opParameters;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;
	node.noStorage_ = true;

	retVec->nodeId_ = vecValuedFct->graph_->AddNode(&node);

	if(Node::ID_NONE == retVec->nodeId_)
	{
		Error("Could not add Node!\n");
		return nullptr;
	}

	return retVec;
}

VectorSpace::Vector* VectorSpace::Vector::Add(const Vector* vec)
{
	if(!AreCompatible(this, vec))
	{
		Error("Incompatible Vectors!\n");
		return nullptr;
	}

	// Infer space
	Ring::type_t inferredRing = Ring::GetSuperiorRing(__space_->GetRing(), vec->__space_->GetRing());
	if(Ring::None == inferredRing)
	{
		Error("Incompatible Rings\n");
		return nullptr;
	}

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;

	if(inferredRing == __space_->GetRing())
	{
		retVec->__space_ = __space_;
	}
	else
	{
		retVec->__space_ = vec->__space_;
	}

	Node node;
	node.parents.push_back(nodeId_);
	node.parents.push_back(vec->nodeId_);
	node.type = Node::Type::VECTOR_ADDITION;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	retVec->nodeId_ = graph_->AddNode(&node);

	if(Node::ID_NONE == retVec->nodeId_)
	{
		Error("Could not add Node!\n");
		return nullptr;
	}

	return retVec;
}

VectorSpace::VectorSpace(const std::vector<simpleVs_t>* factors)
{
	factors_ = *factors;
}

VectorSpace::VectorSpace(std::initializer_list<const VectorSpace*> list)
{
	for(const VectorSpace * vSpace: list)
	{
		factors_.insert(factors_.end(), vSpace->factors_.begin(), vSpace->factors_.end());
	}
}

