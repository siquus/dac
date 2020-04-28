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

template const VectorSpace::Vector * VectorSpace::Element<float>(Graph * graph, const std::vector<float> &initializer) const;

template const VectorSpace::Vector * VectorSpace::Element<float>(
		Graph* graph,
		const std::vector<float> &initializer,
		Vector::Property property,
		const void * parameter) const;

template const VectorSpace::Vector * VectorSpace::Element<float>(
			Graph* graph,
			const std::vector<float> &initializer,
			const std::vector<Vector::Property> &properties,
			const std::vector<const void *> &propertiesParameter) const;

template const VectorSpace::Vector * VectorSpace::Scalar<float>(Graph * graph, const float &initializer) const;
template const VectorSpace::Vector * VectorSpace::Scalar<int32_t>(Graph * graph, const int32_t &initializer) const;
template const VectorSpace::Vector * VectorSpace::Homomorphism<float>(Graph* graph, const std::vector<float> &initializer) const;

template const VectorSpace::Vector * VectorSpace::Homomorphism<float>(
			Graph* graph,
			const std::vector<float> &initializer,
			Vector::Property property,
			const void * parameter) const;  // initializer Pointer is taken

template const VectorSpace::Vector * VectorSpace::Homomorphism<float>(
		Graph* graph,
		const std::vector<float> &initializer,
		const std::vector<Vector::Property> &properties,
		const std::vector<const void *> &propertiesParameter) const;

template const VectorSpace::Vector* VectorSpace::Vector::Power<float>(float exp) const;
template const VectorSpace::Vector* VectorSpace::Vector::Multiply<float>(float factor) const;

template<typename T>
static bool hasDuplicates(const std::vector<T> &vec)
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

static void printVector(const std::vector<uint32_t> &vec)
{
	printf("(");
	for(size_t index = 0; index < vec.size(); index++)
	{
		printf("%u ", vec[index]);
	}
	printf(")");
}

VectorSpace::VectorSpace(Ring::type_t ring, dimension_t dim)
{
	factors_.push_back(simpleVs_t{ring, dim});
}

VectorSpace::VectorSpace(Ring::type_t ring, const std::vector<dimension_t> &dimensions)
{
	for(size_t dim = 0; dim < dimensions.size(); dim++)
	{
		factors_.push_back(simpleVs_t{ring, dimensions[dim]});
	}
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
	for(size_t fac = 0; fac < factors_.size(); fac++)
	{
		uint32_t stride = 1;
		for(size_t prodFac = fac + 1; prodFac < factors_.size(); prodFac++)
		{
			stride *= factors_[prodFac].dim_;
		}

		strides->at(fac) = stride;
	}
}

template<typename inType>
const VectorSpace::Vector * VectorSpace::Homomorphism(
			Graph* graph,
			const std::vector<inType> &initializer,
			Vector::Property property,
			const void * parameter) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	(void)(parameter);

	// Handle properties, where no indices need to be specified
	if((Vector::Property::Diagonal == property) && (1 == factors_.size()))
	{
		std::vector<Vector::Property> props;
		props.push_back(property);

		std::pair<uint32_t, uint32_t> param{0, 1};
		std::vector<const void *> propParam;
		propParam.push_back(&param);

		return Homomorphism(
				graph,
				initializer,
				props,
				propParam);
	}

	Error("Not implemented!\n");
	return nullptr;
}

template<typename inType>
const VectorSpace::Vector * VectorSpace::Homomorphism(
		Graph* graph,
		const std::vector<inType> &initializer,
		const std::vector<Vector::Property> &properties,
		const std::vector<const void *> &propertiesParameter) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	// TODO: Call VectorSpace::Element rather than reimplenting stuff here!

	if(properties.size() != propertiesParameter.size())
	{
		Error("Properties and propertiesParameter need to have same size!\n");
		return nullptr;
	}

	// TODO: Implement properties. For now this is just an interface to test usability in application
	if((properties.size() != 1) ||
			(Vector::Property::Diagonal != properties[0]) ||
			(factors_.size() != 1))
	{
		Error("Property not implemented yet!\n");
		return nullptr;
	}

	// For now, simply create the full matrix
	const size_t dimensions = GetDim();

	auto newInitiliazer = new std::vector<inType>(dimensions * dimensions, 0.f);

	// Fill diagonal values
	for(size_t dim = 0; dim < dimensions; dim++)
	{
		newInitiliazer->at(dim * dimensions + dim) = initializer[dim];
	}

	return Homomorphism(graph, *newInitiliazer);
}

template<typename inType>
const VectorSpace::Vector * VectorSpace::Homomorphism(Graph* graph, const std::vector<inType> &initializer) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	if(initializer.size() != GetDim() * GetDim())
	{
		Error("Initializer dimensions do not match (%lu vs %i)!\n",
				initializer.size(), GetDim() * GetDim());
		return nullptr;
	}

	if(!Ring::IsCompatible(GetRing(), initializer))
	{
		Error("Initializer for Homomorphism of incompatible Type!\n");
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

	VectorSpace * retSpace = new VectorSpace(*this, 2);

	retVec->__value_ = initializer.data();
	retVec->__space_ = retSpace;
	retVec->graph_ = graph;
	retVec->nodeId_ = nodeId;

	return retVec;
}

template<typename inType>
const VectorSpace::Vector * VectorSpace::Scalar(Graph* graph, const inType &initializer) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	if(!Ring::IsCompatible(GetRing(), initializer))
	{
		Error("Initializer for Scalar of incompatible Type!\n");
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

	retVec->__space_ = new VectorSpace(GetRing(), 1);

	void * valuePt = malloc(sizeof(initializer));
	memcpy(valuePt, &initializer, sizeof(initializer));

	retVec->__value_ = valuePt;
	retVec->graph_ = graph;
	retVec->nodeId_ = nodeId;

	return retVec;
}

template<typename inType>
const VectorSpace::Vector * VectorSpace::Element(
			Graph* graph,
			const std::vector<inType> &initializer,
			Vector::Property property,
			const void * parameter) const
{
	// TODO: Implement properties. For now this is just an interface to test usability in application
	if(Vector::Property::Sparse != property)
	{
		Error("Not implemented!\n");
		return nullptr;
	}

	if(nullptr == parameter)
	{
		Error("Nullpointer!\n");
		return nullptr;
	}

	const Vector::propertyParameterSparse_t * param = (const Vector::propertyParameterSparse_t *) parameter;
	if(param->DENSE != param->Initializer)
	{
		Error("Not implemented!\n");
		return nullptr;
	}

	// Ignore sparse..

	return Element(graph, initializer);
}

template<typename inType>
const VectorSpace::Vector * VectorSpace::Element(
			Graph* graph,
			const std::vector<inType> &initializer,
			const std::vector<Vector::Property> &properties,
			const std::vector<const void *> &propertiesParameter) const
{
	// TODO: Implement properties. For now this is just an interface to test usability in application
	(void)(propertiesParameter);

	if((2 == properties.size()) &&
			(properties[0] == Vector::Property::Sparse) &&
			(properties[1] == Vector::Property::Antisymmetric))
	{
		// Ignore properties
		return Element(graph, initializer);
	}

	Error("Not implemented!\n");
	return nullptr;
}

template<typename inType>
const VectorSpace::Vector * VectorSpace::Element(Graph * graph, const std::vector<inType>  &initializer) const
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

const VectorSpace::Vector * VectorSpace::Element(Graph* graph, const std::vector<uint32_t> &DeltaPairs, float Scaling) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	if(DeltaPairs.size() != factors_.size())
	{
		Error("Initializer dimensions do not match (%lu vs %lu)!\n",
				DeltaPairs.size(), factors_.size());
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

	Node::KroneckerDeltaParameters_t * param = new Node::KroneckerDeltaParameters_t;
	param->DeltaPair = DeltaPairs;
	param->Scaling = Scaling;

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

bool VectorSpace::AreEqual(const VectorSpace * lVs, const VectorSpace * rVs)
{
	if(lVs->factors_.size() != rVs->factors_.size())
	{
		return false;
	}

	for(size_t factor = 0; factor < lVs->factors_.size(); factor++)
	{
		if(memcmp(&lVs->factors_[factor], &rVs->factors_[factor], sizeof(lVs->factors_[factor])))
		{
			return false;
		}
	}

	return true;
}

bool VectorSpace::Vector::SameValue(const Vector * lVec, const Vector * rVec)
{
	if(!AreEqual(lVec->__space_, rVec->__space_))
	{
		return false;
	}

	if(((lVec->__value_ == nullptr) && (rVec->__value_ != nullptr)) ||
			((lVec->__value_ != nullptr) && (rVec->__value_ == nullptr)))
	{
		return false;
	}

	if(lVec->__value_ == nullptr)
	{
		return true; // by above, both are nullptr
	}

	size_t cmpSize = 0;
	for(const auto &factor: lVec->__space_->factors_)
	{
		cmpSize += factor.dim_ * Ring::GetElementSize(factor.ring_);
	}

	if(memcmp(lVec->__value_, rVec->__value_, cmpSize))
	{
		return false;
	}

	return true;
}

void VectorSpace::Vector::PrintInfo() const {

	const Node* thisNode = graph_->GetNode(nodeId_);
	if(nullptr == thisNode)
	{
		Error("Could not find node!\n");
	}

	printf("NodeId: %u, Type: %s, Factor-Dim. (",
			nodeId_,
			thisNode->getName()	);

	for(size_t factor = 0; factor < __space_->factors_.size(); factor++)
	{
		printf("%u ", __space_->factors_[factor].dim_);
	}
	printf(")");

	printf("\n");
}

const VectorSpace::Vector* VectorSpace::Vector::Power(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors) const
{
	// TODO: Catch exponent of 0?
	if(graph_ != vec->graph_)
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	bool rArgScalar = (1 == vec->__space_->GetDim());
	if(!rArgScalar)
	{
		Error("Can not take something to the power of a non-scalar!\n");
		return nullptr;
	}

	if(lfactors.size() != rfactors.size())
	{
		Error("Contraction indice-vectors of different size!\n");
		return nullptr;
	}

	// The contraction needs to return the same tensor again, otherwise the contraction can't be repeated!
	// I.e. 1/2 * (nr of factors) == (nr of contractions)
	if(__space_->factors_.size() != 2 * lfactors.size())
	{
		Error("Can only take the power, if half of the indices are contracted!\n");
		return nullptr;
	}

	// Check if indices are inside allowed range
	for(size_t index = 0; index < lfactors.size(); index++)
	{
		if((__space_->factors_.size() <= lfactors[index]) || (__space_->factors_.size() <= rfactors[index]))
		{
			Error("At least one given contraction factor is larger than number of factors!\n");
			return nullptr;
		}

		if(__space_->factors_[lfactors[index]].dim_ != __space_->factors_[rfactors[index]].dim_)
		{
			Error("At least one contraction index-pair has different dimension! Factor %u with %u, |lFactor| = %u, |rFactor| = %u\n",
					lfactors[index], rfactors[index],
					__space_->factors_[lfactors[index]].dim_,
					vec->__space_->factors_[rfactors[index]].dim_);

			return nullptr;
		}
	}

	// Check for dublicate indices
	if(hasDuplicates(lfactors) || hasDuplicates(rfactors))
	{
		Error("Dublicate contraction factors!\n");
		return nullptr;
	}


	Vector* retVec = new Vector;
	retVec->graph_ = graph_;

	retVec->__space_ = __space_;

	Node node;
	node.parents.push_back(nodeId_);
	node.parents.push_back(vec->nodeId_);
	node.type = Node::Type::VECTOR_POWER;
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

template<typename inType>
const VectorSpace::Vector* VectorSpace::Vector::Power(inType exp) const
{
	// TODO: Catch exponent of 0?
	return Power(__space_->Scalar(graph_, exp));
}

const VectorSpace::Vector* VectorSpace::Vector::Power(const Vector* vec) const
{
	// TODO: Catch exponent of 0?

	if(graph_ != vec->graph_)
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	if(1 < vec->__space_->GetDim())
	{
		Error("Can't take power to non-scalar value!\n");
		return nullptr;
	}

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;

	retVec->__space_ = __space_;

	Node node;
	node.parents.push_back(nodeId_);
	node.parents.push_back(vec->nodeId_);
	node.type = Node::Type::VECTOR_POWER;
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

const VectorSpace::Vector* VectorSpace::Vector::Divide(const Vector* vec) const
{
	const Vector * minusOne = vec->__space_->Scalar(vec->graph_, -1.f);
	if(nullptr == minusOne)
	{
		Error("Could not create scalar!\n");
		return nullptr;
	}

	const Vector * oneOverVec = vec->Power(minusOne);
	if(nullptr == oneOverVec)
	{
		Error("Could not take power!\n");
		return nullptr;
	}

	const Vector * Result = Multiply(oneOverVec);
	if(nullptr == Result)
	{
		Error("Could not Multiply!\n");
		return nullptr;
	}

	return Result;
}

template<typename inType>
const VectorSpace::Vector* VectorSpace::Vector::Multiply(inType factor) const
{
	auto factorVec = __space_->Scalar(graph_, factor);

	return Multiply(factorVec);
}

const VectorSpace::Vector* VectorSpace::Vector::Multiply(const Vector* vec) const
{
	if(graph_ != vec->graph_)
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	// Vector - Scalar multiplication: Do not add V-Space factors
	// Rationale for creating an exception by not adding factors: Multiplying scalars would quickly become a confusion of indices.
	bool lArgScalar = (1 == __space_->GetDim());
	bool rArgScalar = (1 == vec->__space_->GetDim());

	if(lArgScalar || rArgScalar)
	{
		// Infer space

		Vector* retVec = new Vector;
		retVec->graph_ = graph_;

		const VectorSpace * retSpace = nullptr;

		if(lArgScalar && rArgScalar)
		{
			Ring::type_t inferredRing = Ring::GetSuperiorRing(__space_->GetRing(), vec->__space_->GetRing());
			if(Ring::None == inferredRing)
			{
				Error("Incompatible Rings\n");
				return nullptr;
			}

			retSpace = new VectorSpace(inferredRing, 1);
		}
		else if(lArgScalar)
		{
			retSpace = vec->__space_;
		}
		else
		{
			retSpace = __space_;
		}

		if(nullptr == retSpace)
		{
			Error("Could not create VectorSpace");
			return nullptr;
		}

		retVec->__space_ = retSpace;

		Node node;
		node.parents.push_back(nodeId_);
		node.parents.push_back(vec->nodeId_);

		node.type = Node::Type::VECTOR_SCALAR_PRODUCT;
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

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;

	VectorSpace * retSpace = nullptr;
	retSpace = new VectorSpace(std::initializer_list<const VectorSpace*>{__space_, vec->__space_});

	if(nullptr == retSpace)
	{
		Error("Could not create VectorSpace");
		return nullptr;
	}

	retVec->__space_ = retSpace;

	Node node;
	node.parents.push_back(nodeId_);
	node.parents.push_back(vec->nodeId_);
	node.type = Node::Type::VECTOR_VECTOR_PRODUCT;
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

const VectorSpace::Vector* VectorSpace::Vector::JoinIndices(std::vector<std::vector<uint32_t>> &indices) const
{
	std::vector<uint32_t> indicesLinear;
	for(const auto &joinedIndices: indices)
	{
		for(const uint32_t &index: joinedIndices)
		{
			if(__space_->factors_.size() <= index)
			{
				Error("Supplied index is larger than available factors!\n");
				return nullptr;
			}

			indicesLinear.push_back(index);
		}
	}

	if(hasDuplicates(indicesLinear))
	{
		Error("Duplicate indices specified!\n");
		return nullptr;
	}

	// Check that all joined indices have same dimension
	for(const auto &joinedIndices: indices)
	{
		const uint32_t indexDim = __space_->factors_[joinedIndices[0]].dim_;
		for(const uint32_t &index: joinedIndices)
		{
			if(indexDim != __space_->factors_[index].dim_)
			{
				Error("Not all specified indices have same dimension!\n");
				return nullptr;
			}
		}
	}

	// Sort joined indices
	for(auto &joinedIndices: indices)
	{
		std::sort(joinedIndices.begin(), joinedIndices.end());
	}

	std::vector<simpleVs_t> vsFactors;
	for(size_t factor = 0; factor < __space_->factors_.size(); factor++)
	{
		// Check if the factor is joined
		bool addFactor = true;
		for(size_t joinedIndices = 0; joinedIndices < indices.size(); joinedIndices++)
		{
			const auto factorIt = std::find(indices[joinedIndices].begin(), indices[joinedIndices].end(), factor);

			if(indices[joinedIndices].end() != factorIt) // This is a joined index
			{
				if(indices[joinedIndices].begin() != factorIt) // Factor for joined index is added at the position of the lowest index
				{
					addFactor = false;
				}

				break; // We found the index, don't need to search in other joinedIndices.
			}
		}

		if(addFactor)
		{
			vsFactors.push_back(__space_->factors_[factor]);
		}
	}

	VectorSpace * retSpace = new VectorSpace(vsFactors);

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;
	retVec->__space_ = retSpace;

	Node node;
	node.parents.push_back(nodeId_);
	node.type = Node::Type::VECTOR_JOIN_INDICES;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	Node::joinIndicesParameters_t * param = new Node::joinIndicesParameters_t;
	param->Indices = indices;
	std::sort(param->Indices.begin(), param->Indices.end());

	node.typeParameters = param;

	retVec->nodeId_ = graph_->AddNode(&node);

	if(Node::ID_NONE == retVec->nodeId_)
	{
		Error("Could not add Node!\n");
		return nullptr;
	}

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::IsSmaller(const Vector* vec) const
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
		Error("Product Space has different number of factors, %lu vs %lu!\n",
				vec1->__space_->factors_.size(), vec2->__space_->factors_.size());
		return false;
	}

	for(size_t factor = 0; factor < vec1->__space_->factors_.size(); factor++)
	{
		if(vec1->__space_->factors_[factor].dim_ != vec2->__space_->factors_[factor].dim_)
		{
			Error("Factor %lu is of different dimension!\n", factor);
			return false;
		}

		if(vec1->__space_->factors_[factor].ring_ != vec2->__space_->factors_[factor].ring_)
		{
			Error("Factor %lu has a different ring!\n", factor);
			return false;
		}
	}

	return true;
}

const VectorSpace::Vector * VectorSpace::Vector::Derivative(const Vector* vec) const
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

const VectorSpace::Vector* VectorSpace::Vector::CreateDerivative(std::map<Node::Id_t, depNode_t> * depNodes, const VectorSpace::Vector * currentVec, Node::Id_t depNodeId) const
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
	const Vector * summandVec = nullptr;
	for(const Node::Id_t &parentId: (*depNodes)[currentVec->nodeId_].parents)
	{
		const Node * parentNode = graph_->GetNode(parentId);
		if(nullptr == parentNode)
		{
			Error("Unknown Node Id!\n");
			return nullptr;
		}

		const Node* fctNode = currentVec->graph_->GetNode(currentVec->nodeId_);
		printf("DepNode %u ParNode %u: Calculating derivative of %s (id %u) w.r.t. %s (id %u)\n",
				depNodeId, parentId,
				fctNode->getName(), fctNode->id,
				parentNode->getName(), parentNode->id);

		if(Node::ObjectType::MODULE_VECTORSPACE_VECTOR != parentNode->objectType)
		{
			Error("Can't take derivative w.r.t. non-vector node of object type %u!\n",
					(uint8_t) parentNode->objectType);
			return nullptr;
		}

		// TODO: Check for Null returns
		const Vector * parentVec = (const Vector *) parentNode->object;

		const Vector * derivativeVec = CreateDerivative(currentVec, parentVec);
		if(nullptr == derivativeVec)
		{
			Error("Could not get derivative of %s!\n", parentNode->getName());
		}

		if(depNodeId != parentId)
		{
			printf("\nTaking derivative of:\n");
			parentVec->PrintInfo();

			// Call this function recursively if parent is not the variable w.r.t. which
			// the derivative is calculated
			const Vector * innerDerivative = CreateDerivative(depNodes, parentVec, depNodeId);
			if(nullptr == innerDerivative)
			{
				Error("Failed to CreateDerivative!\n");
				return nullptr;
			}

			printf("Got:\n");
			innerDerivative->PrintInfo();
			printf("\n");

			if(1 != derivativeVec->__space_->GetDim()) // i.e. if a scalar function is derived w.r.t. vector argument
			{
				std::vector<uint32_t> lfactors(parentVec->__space_->factors_.size());
				std::iota(lfactors.begin(), lfactors.end(),
						innerDerivative->__space_->factors_.size() - parentVec->__space_->factors_.size());

				std::vector<uint32_t> rfactors(parentVec->__space_->factors_.size());
				std::iota(rfactors.begin(), rfactors.end(), 0);

				printf("Contracting ");
				printVector(lfactors);
				printVector(rfactors);
				printf(":\n");

				innerDerivative->PrintInfo();
				derivativeVec->PrintInfo();
				printf("\n");

				derivativeVec = innerDerivative->Contract(
						derivativeVec,
						lfactors, rfactors);

				if(nullptr == derivativeVec)
				{
					Error("Contraction failed!\n");
					return nullptr;
				}
			}
			else
			{
				derivativeVec = innerDerivative->Multiply(derivativeVec);
			}
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
			Error("Could not create derivative! Failed at %s.\n", parentNode->getName());
			return nullptr;
		}
	}

	return summandVec;
}

// For a function V_0 x V_1 .. x V_n -> V, this function creates the derivative,
// i.e. it returns a vector in the space V_arg \tensor V
const VectorSpace::Vector* VectorSpace::Vector::CreateDerivative(const Vector* vecValuedFct, const Vector* arg)
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

	case Node::Type::VECTOR_CONTRACTION:
		return ContractDerivative(vecValuedFct, arg);

	case Node::Type::VECTOR_PERMUTATION:
		return PermuteDerivative(vecValuedFct, arg);

	case Node::Type::VECTOR_SCALAR_PRODUCT:	// no break intended
	case Node::Type::VECTOR_VECTOR_PRODUCT:
		return MultiplyDerivative(vecValuedFct, arg);

	case Node::Type::VECTOR_POWER:
		return PowerDerivative(vecValuedFct, arg);

	case Node::Type::VECTOR_PROJECTION:
		return ProjectDerivative(vecValuedFct, arg);

	default:
		Error("Node Type %s does not support taking its derivative!\n", Node::getName(fctNode->type));
		return nullptr;
	}


	return nullptr;
}

const VectorSpace::Vector* VectorSpace::Vector::Contract(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors) const
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

	if(0 == lfactors.size())
	{
		// This is not a contraction but a tensor product!
		return Multiply(vec);
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
			Error("At least one contraction index-pair has different dimension! Factor %u with %u, |lFactor| = %u, |rFactor| = %u\n",
					lfactors[index], rfactors[index],
					__space_->factors_[lfactors[index]].dim_,
					vec->__space_->factors_[rfactors[index]].dim_);

			return nullptr;
		}
	}

	// Check for dublicate indices
	if(hasDuplicates(lfactors) || hasDuplicates(rfactors))
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
	retSpace = new VectorSpace(factorsVec);
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
			(Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT == vecNode->type))
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

		const Node::KroneckerDeltaParameters_t * thisKronParam = (const Node::KroneckerDeltaParameters_t *) thisNode->typeParameters;
		const Node::KroneckerDeltaParameters_t * vecKronParam = (const Node::KroneckerDeltaParameters_t *) vecNode->typeParameters;

		Node::KroneckerDeltaParameters_t * opKronParam = new Node::KroneckerDeltaParameters_t;
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
		Node::contractParameters_t * opParameters = new Node::contractParameters_t;
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

const VectorSpace::Vector* VectorSpace::Vector::Contract(const Vector* vec, uint32_t lfactor, uint32_t rfactor) const
{
	std::vector<uint32_t> lfactors{lfactor};
	std::vector<uint32_t> rfactors{rfactor};

	return Contract(vec, lfactors, rfactors);
}

const VectorSpace::Vector* VectorSpace::Vector::Project(const std::pair<uint32_t, uint32_t> &range) const
{
	if(1 != __space_->factors_.size())
	{
		Error("Specify range for each factor!\n");
		return nullptr;
	}

	std::vector<std::pair<uint32_t, uint32_t>> IndexRange;
	IndexRange.push_back(range);

	return Project(IndexRange);
}

const VectorSpace::Vector* VectorSpace::Vector::Project(const std::vector<std::pair<uint32_t, uint32_t>> &range) const
{
	if(range.size() != __space_->factors_.size())
	{
		Error("Specify range for each factor!\n");
		return nullptr;
	}

	for(size_t factorRange = 0; factorRange < range.size(); factorRange++)
	{
		if((__space_->factors_[factorRange].dim_ < range[factorRange].first) ||
				(__space_->factors_[factorRange].dim_ < range[factorRange].second))
		{
			Error("Factor range is larger than factor dimension!\n");
			return nullptr;
		}

		if(range[factorRange].first >= range[factorRange].second)
		{
			Error("Lower factor range is larger than / equal to upper factor range!\n");
			return nullptr;
		}
	}

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;

	std::vector<simpleVs_t> newFactors = __space_->factors_;
	for(size_t factor = 0; factor < newFactors.size(); factor++)
	{
		newFactors[factor].dim_ = range[factor].second - range[factor].first;
	}

	VectorSpace * retSpace = new VectorSpace(newFactors);
	retVec->__space_ = retSpace;

	Node node;
	node.parents.push_back(nodeId_);
	node.type = Node::Type::VECTOR_PROJECTION;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	Node::projectParameters_t * opParameters = new Node::projectParameters_t;
	opParameters->range = range;

	node.typeParameters = opParameters;

	retVec->nodeId_ = graph_->AddNode(&node);

	if(Node::ID_NONE == retVec->nodeId_)
	{
		Error("Could not add Node!\n");
		return nullptr;
	}

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::Permute(const std::vector<uint32_t> &indices) const
{
	if(hasDuplicates(indices))
	{
		Error("Dublicate permute indices!\n");
		return nullptr;
	}

	for(const auto &index: indices)
	{
		if(index >= __space_->factors_.size())
		{
			Error("Index is larger than number of factors!\n");
		}
	}

	if(indices.size() != __space_->factors_.size())
	{
		Error("Number of permutation indices does not match number of factors!\n");
	}

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;
	retVec->__space_ = __space_;

	Node node;
	node.parents.push_back(nodeId_);
	node.type = Node::Type::VECTOR_PERMUTATION;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	Node::permuteParameters_t * opParameters = new Node::permuteParameters_t;
	opParameters->indices = indices;

	node.typeParameters = opParameters;

	retVec->nodeId_ = graph_->AddNode(&node);

	if(Node::ID_NONE == retVec->nodeId_)
	{
		Error("Could not add Node!\n");
		return nullptr;
	}

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::ProjectDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	if(Ring::Float32 != arg->__space_->GetRing())
	{
		Error("Non implemented!\n");
		return nullptr;
	}

	const Node * fctNode = vecValuedFct->graph_->GetNode(vecValuedFct->nodeId_);
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Node::projectParameters_t * projParam = (const Node::projectParameters_t *) fctNode->typeParameters;

	auto retSpace = new VectorSpace(std::vector<const VectorSpace*>{arg->__space_, vecValuedFct->__space_});

	// Create new vector "blackWhiteArg" of arg-type: All proj. values are = 1, others = 0
	// Construct dProjection/darg = delta_{darg-indices, arg-indices} blackWhiteArg_{arg-indices} (no sum)
	// TODO: Have to construct this manually. Would be cheaper to have an operation
	// delta_ij a_jkl (no sum over j). Note: This operation would use a delta on indices with different dim
	// Or to use a sparse initializer.
	auto initializer = new std::vector<float>(retSpace->GetDim(), 0);

	std::vector<uint32_t> strides;
	retSpace->GetStrides(&strides);

	for(size_t index = 0; index < initializer->size(); index++)
	{
		std::vector<uint32_t> coord(retSpace->factors_.size());
		coord[0] = index / strides[0];

		for(size_t factor = 1; factor < retSpace->factors_.size(); factor++)
		{
			coord[factor] = (index % strides[factor - 1]) / strides[factor];
		}

		// The arg part of the coordinate starts at 0. Add its offset to get "pre-Projection" coordinate
		const size_t coordMiddle = coord.size() / 2;
		for(size_t argFactor = 0; argFactor < projParam->range.size(); argFactor++)
		{
			coord[argFactor + coordMiddle] += projParam->range[argFactor].first;
		}

		if(std::equal(coord.begin(), coord.end() - coordMiddle, coord.end() - coordMiddle))
		{
			bool inRange = true;
			for(size_t factor = 0; factor < projParam->range.size(); factor++)
			{
				if((projParam->range[factor].first > coord[factor]) ||
						(projParam->range[factor].second < coord[factor]))
				{
					inRange = false;
					break;
				}
			}

			if(inRange)
			{
				initializer->at(index) = 1;
			}
		}
	}

	propertyParameterSparse_t paramSparse;
	paramSparse.Initializer = paramSparse.DENSE;

	const VectorSpace::Vector* retVec = retSpace->Element(
			arg->graph_,
			*initializer,
			Property::Sparse,
			&paramSparse);

	if(nullptr == retVec)
	{
		Error("Could not create vector!\n");
		return nullptr;
	}

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::PowerDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	// TODO: Catch exponent of 1?

	const Node * fctNode = vecValuedFct->graph_->GetNode(vecValuedFct->nodeId_);
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Node * lNode = arg->graph_->GetNode(fctNode->parents[0]);
	if(nullptr == lNode)
	{
		Error("Could not find node Id%u!\n", fctNode->parents[0]);
		return nullptr;
	}

	const Node * rNode = arg->graph_->GetNode(fctNode->parents[1]);
	if(nullptr == rNode)
	{
		Error("Could not find node Id%u!\n", fctNode->parents[1]);
		return nullptr;
	}

	const Vector* baseVector = (const Vector *) lNode->object;
	const Vector* expVector = (const Vector *) rNode->object;

	bool derivativeWrtBase = (fctNode->parents[0] == arg->nodeId_);

	if(!derivativeWrtBase)
	{
		Error("Taking derivative w.r.t. exponent is not implemented!\n");
		return nullptr;
	}

	// d/db b^e = e * b^(e-1)
	const Vector* minusOne = expVector->__space_->Scalar(expVector->graph_, -1.f);
	if(nullptr == minusOne)
	{
		Error("Could not create scalar!\n");
		return nullptr;
	}

	const Vector * exponent = expVector->Add(minusOne);
	if(nullptr == exponent)
	{
		Error("Could not add!\n");
		return nullptr;
	}

	const Vector* power = baseVector->Power(exponent);
	if(nullptr == power)
	{
		Error("Could not take power!\n");
		return nullptr;
	}

	const Vector* derivative = expVector->Multiply(power);
	if(nullptr == derivative)
	{
		Error("Could not multiply!\n");
		return nullptr;
	}

	// If this is the scalar case, we are done!
	if(1 == derivative->__space_->GetDim())
	{
		return derivative;
	}

	// d(a_ijk^2) / d(a_lmn) = 2 * delta_li * delta_mj * delta_nk * a_ijk (no sum)
	// Multiply with Kronecker
	std::vector<uint32_t> deltaPairs(2 * arg->__space_->factors_.size());
	for(size_t deltaFactor = 0; deltaFactor < deltaPairs.size() / 2; deltaFactor++)
	{
		deltaPairs[deltaFactor] = deltaFactor + deltaPairs.size() / 2;
		deltaPairs[deltaFactor + deltaPairs.size() / 2] = deltaFactor;
	}

	VectorSpace * kronVectorSpace = new VectorSpace(*arg->__space_, 2);

	const Vector * kronVec = kronVectorSpace->Element(arg->graph_, deltaPairs);
	if(nullptr == kronVec)
	{
		Error("Could not create Kronecker\n");
		return nullptr;
	}

	derivative = kronVec->Multiply(derivative);
	if(nullptr == derivative)
	{
		Error("Could not multiply\n");
		return nullptr;
	}

	// The second half of the Kronecker indices join the function indices in the back
	std::vector<std::vector<uint32_t>> indicesToJoin;
	for(size_t FacToJoin = 0; FacToJoin < arg->__space_->factors_.size(); FacToJoin++)
	{
		indicesToJoin.push_back(
				std::vector<uint32_t>{
					(uint32_t) (arg->__space_->factors_.size() + FacToJoin),
					(uint32_t) (2 * arg->__space_->factors_.size() + FacToJoin)});
	}


	derivative = derivative->JoinIndices(indicesToJoin);

	return derivative;
}

const VectorSpace::Vector* VectorSpace::Vector::MultiplyDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	// Is arg the right side, i.e. the scalar?
	// On which side of the contraction is the non-arg node?
	const Node * fctNode = vecValuedFct->graph_->GetNode(vecValuedFct->nodeId_);
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	bool argOnRightSide;
	Node::Id_t otherNodeId;
	if(fctNode->parents[0] == arg->nodeId_)
	{
		argOnRightSide = false;
		otherNodeId = fctNode->parents[1];
	}
	else
	{
		argOnRightSide = true;
		otherNodeId = fctNode->parents[0];
	}

	const Node * otherNode = arg->graph_->GetNode(otherNodeId);
	if(nullptr == otherNode)
	{
		Error("Could not find node Id%u!\n", otherNodeId);
		return nullptr;
	}

	const Vector * otherVec = (const Vector *) otherNode->object;

	// Vector - Scalar multiplication: Do not add V-Space factors
	// Rationale for creating an exception by not adding factors: Multiplying scalars would quickly become a confusion of indices.
	bool argScalar = (1 == arg->__space_->GetDim());

	if(argScalar)
	{
		return otherVec;
	}

	std::vector<uint32_t> deltaPairs(2 * arg->__space_->factors_.size());
	for(size_t deltaFactor = 0; deltaFactor < deltaPairs.size() / 2; deltaFactor++)
	{
		deltaPairs[deltaFactor] = deltaFactor + deltaPairs.size() / 2;
		deltaPairs[deltaFactor + deltaPairs.size() / 2] = deltaFactor;
	}

	VectorSpace * kronVectorSpace = new VectorSpace(*arg->__space_, 2);

	const Vector * kronVec = kronVectorSpace->Element(arg->graph_, deltaPairs);
	if(nullptr == kronVec)
	{
		Error("Could not create Kronecker\n");
		return nullptr;
	}

	const Vector * Product = kronVec->Multiply(otherVec);
	if(nullptr == kronVec)
	{
		Error("Could not multiply\n");
		return nullptr;
	}

	if(!argOnRightSide)
	{
		// Arg is on left side: No reordering of indices necessary
		return Product;
	}

	// Now we just have to reorder the indices..

	// Create Permutation:
	std::vector<uint32_t> permutation(Product->__space_->factors_.size());
	std::iota(permutation.begin(), permutation.end(), 0);

	// Move other factors to the left
	for(uint32_t otherFactor = 0; otherFactor < otherVec->__space_->factors_.size(); otherFactor++)
	{
		permutation[arg->__space_->factors_.size() + otherFactor] = arg->__space_->factors_.size() + otherFactor + arg->__space_->factors_.size();
	}

	// Move Arg factors to the right
	for(uint32_t argFactor = 0; argFactor < arg->__space_->factors_.size(); argFactor++)
	{
		permutation[arg->__space_->factors_.size() + otherVec->__space_->factors_.size() + argFactor] = arg->__space_->factors_.size() + argFactor;
	}

	return Product->Permute(permutation);
}

const VectorSpace::Vector* VectorSpace::Vector::PermuteDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	const Node * fctNode = vecValuedFct->graph_->GetNode(vecValuedFct->nodeId_);
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Node::permuteParameters_t * permutation = (const Node::permuteParameters_t *) fctNode->typeParameters;

	// The result will just be a Kronecker product
	std::vector<uint32_t> deltaPairs(2 * arg->__space_->factors_.size());
	for(size_t deltaFactor = 0; deltaFactor < deltaPairs.size() / 2; deltaFactor++)
	{
		deltaPairs[deltaFactor] = permutation->indices[deltaFactor] + deltaPairs.size() / 2;
		deltaPairs[permutation->indices[deltaFactor] + deltaPairs.size() / 2] = deltaFactor;
	}

	VectorSpace * kronVectorSpace = new VectorSpace(*arg->__space_, 2);

	return kronVectorSpace->Element(arg->graph_, deltaPairs);
}

const VectorSpace::Vector* VectorSpace::Vector::ContractDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	// C_ijkmn = d/dB_ij D_kmn = d/dB_ij (A_klm B_ln) = delta(i,l) delta(j,n) A_klm
	// So basically, for each of arg's indices, we'll get a delta
	// Now, care needs to be taken with the index ordering: If we take our contraction algorithm
	// Contract(delta(i,l)delta(j,n), A_klm) = C_ijnkm != C_ijkmn
	// So what we need to do is:
	// E_kmijn = Contract(A, delta(..))
	// C_ijkmn = Permute(E_kmijn)

	// On which side of the contraction is the non-arg node?
	const Node * fctNode = vecValuedFct->graph_->GetNode(vecValuedFct->nodeId_);
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Node::contractParameters_t * contractValue = (const Node::contractParameters_t *) fctNode->typeParameters;

	const std::vector<uint32_t> * argContrFactors;
	const std::vector<uint32_t> * otherContrFactors;
	const Node * otherNode = nullptr;
	bool argOnRightSide;
	if(fctNode->parents[0] == arg->nodeId_)
	{
		argContrFactors = &contractValue->lfactors;
		otherContrFactors = &contractValue->rfactors;
		otherNode = vecValuedFct->graph_->GetNode(fctNode->parents[1]);
		argOnRightSide = false;
	}
	else
	{
		otherContrFactors = &contractValue->lfactors;
		argContrFactors = &contractValue->rfactors;
		otherNode = vecValuedFct->graph_->GetNode(fctNode->parents[0]);
		argOnRightSide = true;
	}

	if(nullptr == otherNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Vector * otherVec = (const Vector *) otherNode->object;

	// Create the Kronecker the otherVec (i.e. non-arg factor of contraction) will be contracted with
	// d/dB_lmn (A_opqr B_ops) = d(l,o) d(m,p) d(n,s) A_opqr
	std::vector<uint32_t> deltaPairs(2 * arg->__space_->factors_.size());
	for(size_t deltaFactor = 0; deltaFactor < deltaPairs.size() / 2; deltaFactor++)
	{
		deltaPairs[deltaFactor] = deltaFactor + deltaPairs.size() / 2;
		deltaPairs[deltaFactor + deltaPairs.size() / 2] = deltaFactor;
	}

	VectorSpace * kronVectorSpace = new VectorSpace(*arg->__space_, 2);
	const Vector * kronVec = kronVectorSpace->Element(arg->graph_, deltaPairs);

	std::vector<uint32_t> lFactors;
	lFactors.resize(otherContrFactors->size());

	for(size_t factor = 0; factor < lFactors.size(); factor++)
	{
		lFactors[factor] = argContrFactors->at(factor) + arg->__space_->factors_.size();
	}

	const Vector* returnVec = kronVec->Contract(otherVec, lFactors, *otherContrFactors);

	// Now we only have one problem: The indices of arg that were not contracted!
	// That is, we now have a vector, D, with wrong ordering:
	// C_lmnqrs := d/dB_lmn (A_opqr B_ops) = d(l,o) d(m,p) d(n,s) A_opqr = D_lmnsqr
	// This can only happen when arg was on the right side of the contraction:
	if(argOnRightSide)
	{
		// All of arg's non-contracted indices need to be moved to the right
		// But since we know the position in the contraction result above, we only need to know how many there are
		uint32_t nrOfUncontractedArgFactors = arg->__space_->factors_.size() - argContrFactors->size();

		// Create Permutation:
		std::vector<uint32_t> permutation(returnVec->__space_->factors_.size());
		std::iota(permutation.begin(), permutation.end(), 0);

		for(uint32_t uncontracted = 0; uncontracted < nrOfUncontractedArgFactors; uncontracted++)
		{
			permutation[arg->__space_->factors_.size() + uncontracted] = permutation.size() - nrOfUncontractedArgFactors + uncontracted;
			permutation[permutation.size() - nrOfUncontractedArgFactors + uncontracted] = arg->__space_->factors_.size() + uncontracted;
		}

		returnVec = returnVec->Permute(permutation);
	}

	return returnVec;
}

const VectorSpace::Vector* VectorSpace::Vector::AddDerivative(const Vector* vecValuedFct, const Vector* arg)
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

	bool argIsScalar = (1 == arg->__space_->GetDim());
	if(argIsScalar)
	{
		// Do not add indices with Kronecker, but simply return identity.
		return vecValuedFct->__space_->Scalar(vecValuedFct->graph_, 1.0f); // TODO: Only works for float
	}

	retVec->__space_ = new VectorSpace(factors);

	// General derivative of Add is easy: Just the product of Kronecker Deltas
	Node node;
	node.type = Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT;
	Node::KroneckerDeltaParameters_t * opParameters = new Node::KroneckerDeltaParameters_t;
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

const VectorSpace::Vector* VectorSpace::Vector::Subtract(const Vector* vec) const
{
	if(!AreCompatible(this, vec))
	{
		Error("Incompatible Vectors!\n");
		return nullptr;
	}

	const Vector * minusOne = vec->__space_->Scalar(vec->graph_, -1.f);
	if(nullptr == minusOne)
	{
		Error("Could not create scalar!\n");
		return nullptr;
	}

	const Vector * minusVec = minusOne->Multiply(vec);
	if(nullptr == minusVec)
	{
		Error("Could not multiply!\n");
		return nullptr;
	}

	const Vector * retVec = Add(minusVec);
	if(nullptr == retVec)
	{
		Error("Could not add!\n");
		return nullptr;
	}

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::Add(const Vector* vec) const
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

VectorSpace::VectorSpace(const std::vector<simpleVs_t> &factors)
{
	factors_ = factors;
}

VectorSpace::VectorSpace(std::vector<const VectorSpace*> vSpaces)
{
	for(const VectorSpace * vSpace: vSpaces)
	{
		factors_.insert(factors_.end(), vSpace->factors_.begin(), vSpace->factors_.end());
	}
}

VectorSpace::VectorSpace(const VectorSpace &vSpace, size_t nTimes)
{
	while(nTimes--)
	{
		factors_.insert(factors_.end(), vSpace.factors_.begin(), vSpace.factors_.end());
	}
}

