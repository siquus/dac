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
			const std::map<Vector::Property, const void *> &properties) const;

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
		const std::map<Vector::Property, const void *> &properties) const;

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
	Factors_.push_back(simpleVs_t{ring, dim});
}

VectorSpace::VectorSpace(Ring::type_t ring, const std::vector<dimension_t> &dimensions)
{
	for(size_t dim = 0; dim < dimensions.size(); dim++)
	{
		Factors_.push_back(simpleVs_t{ring, dimensions[dim]});
	}
}

dimension_t VectorSpace::GetDim() const
{
	dimension_t retDim = 1;
	for(const simpleVs_t &factor: Factors_)
	{
		retDim *= factor.Dim;
	}

	return retDim;
}

Ring::type_t VectorSpace::GetRing() const
{
	Ring::type_t retRing = Ring::None;
	for(const simpleVs_t &factor: Factors_)
	{
		retRing = Ring::GetSuperiorRing(retRing, factor.Ring);
	}

	return retRing;
}

void VectorSpace::GetStrides(std::vector<uint32_t> * strides) const
{
	strides->resize(Factors_.size());
	for(size_t fac = 0; fac < Factors_.size(); fac++)
	{
		uint32_t stride = 1;
		for(size_t prodFac = fac + 1; prodFac < Factors_.size(); prodFac++)
		{
			stride *= Factors_[prodFac].Dim;
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

	// Handle properties, where no indices need to be specified
	if((Vector::Property::Diagonal == property) && (1 == Factors_.size()))
	{
		return Homomorphism(
				graph,
				initializer,
				std::map<Vector::Property, const void *>{{property, parameter}});
	}

	Error("Not implemented!\n");
	return nullptr;
}

template<typename inType>
const VectorSpace::Vector * VectorSpace::Homomorphism(
		Graph* graph,
		const std::vector<inType> &initializer,
		const std::map<Vector::Property, const void *> &properties) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	// TODO: Call VectorSpace::Element rather than reimplenting stuff here!

	// TODO: Implement properties. For now this is just an interface to test usability in application
	if((properties.size() != 1) ||
			(properties.end() == properties.find(Vector::Property::Diagonal)) ||
			(Factors_.size() != 1))
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

	VectorSpace * retSpace = new VectorSpace(*this, 2);

	Vector * retVec = new Vector(graph, retSpace, initializer.data());
	if(nullptr == retVec)
	{
		Error("Could not malloc Vec\n");
		return nullptr;
	}

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

	auto retSpace = new VectorSpace(GetRing(), 1);

	void * valuePt = malloc(sizeof(initializer));
	memcpy(valuePt, &initializer, sizeof(initializer));

	Vector * retVec = new Vector(graph, retSpace, valuePt);
	if(nullptr == retVec)
	{
		Error("Could not malloc Vec\n");
		return nullptr;
	}

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
			const std::map<Vector::Property, const void *> &properties) const
{
	// TODO: Implement properties. For now this is just an interface to test usability in application

	if((2 == properties.size()) &&
			(properties.end() != properties.find(Vector::Property::Sparse)) &&
			(properties.end() != properties.find(Vector::Property::Antisymmetric)))
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

	Vector * retVec = new Vector(graph, this, initializer.data());
	if(nullptr == retVec)
	{
		Error("Could not malloc Vec\n");
		return nullptr;
	}

	return retVec;
}

const VectorSpace::Vector * VectorSpace::Element(Graph* graph, const std::vector<uint32_t> &DeltaPairs, float Scaling) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	if(DeltaPairs.size() != Factors_.size())
	{
		Error("Initializer dimensions do not match (%lu vs %lu)!\n",
				DeltaPairs.size(), Factors_.size());
		return nullptr;
	}

	auto typeParam = new Node::KroneckerDeltaParameters_t{
		DeltaPairs,
		Scaling};

	Vector * retVec = new Vector(
			graph, this,
			Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT, typeParam);

	if(nullptr == retVec)
	{
		Error("Could not malloc Vec\n");
		return nullptr;
	}

	return retVec;
}

const VectorSpace::Vector * VectorSpace::Element(Graph* graph, const std::map<Vector::Property, const void *> &properties) const
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	const auto externalInput = properties.find(Vector::Property::ExternalInput);
	if(properties.end() == externalInput)
	{
		Error("This overloaded Element call requires Property::ExternalInput!");
	}

	Vector * retVec = new Vector(graph, this, properties);
	if(nullptr == retVec)
	{
		Error("Could not malloc Vec\n");
		return nullptr;
	}

	auto externalInputPt = (const Vector::propertyExternalInput_t *) externalInput->second;
	retVec->PushParent(externalInputPt->InputNode);

	return retVec;
}

bool VectorSpace::AreEqual(const VectorSpace * lVs, const VectorSpace * rVs)
{
	if(lVs->Factors_.size() != rVs->Factors_.size())
	{
		return false;
	}

	for(size_t factor = 0; factor < lVs->Factors_.size(); factor++)
	{
		if(memcmp(&lVs->Factors_[factor], &rVs->Factors_[factor], sizeof(lVs->Factors_[factor])))
		{
			return false;
		}
	}

	return true;
}

bool VectorSpace::Vector::SameValue(const Vector * lVec, const Vector * rVec)
{
	if(!AreEqual(lVec->Space_, rVec->Space_))
	{
		return false;
	}

	if(((lVec->Value_ == nullptr) && (rVec->Value_ != nullptr)) ||
			((lVec->Value_ != nullptr) && (rVec->Value_ == nullptr)))
	{
		return false;
	}

	if(lVec->Value_ == nullptr)
	{
		return true; // by above, both are nullptr
	}

	size_t cmpSize = 0;
	for(const auto &factor: lVec->Space_->Factors_)
	{
		cmpSize += factor.Dim * Ring::GetElementSize(factor.Ring);
	}

	if(memcmp(lVec->Value_, rVec->Value_, cmpSize))
	{
		return false;
	}

	return true;
}

void VectorSpace::Vector::PrintInfo() const {

	const Node* thisNode = GetGraph()->GetNode(Id());
	if(nullptr == thisNode)
	{
		Error("Could not find node!\n");
	}

	printf("NodeId: %u, Type: %s, Factor-Dim. (",
			Id(),
			thisNode->getName()	);

	for(size_t factor = 0; factor < Space_->Factors_.size(); factor++)
	{
		printf("%u ", Space_->Factors_[factor].Dim);
	}
	printf(")");

	printf("\n");
}

bool VectorSpace::Vector::Init(
		Graph* graph,
		const VectorSpace * vSpace,
		const void * value,
		const std::map<Property, const void *> &properties,
		Node::Type type, void * typeParam)
{
	Space_ = vSpace;
	Value_ = value;

	Properties_ = properties;

	Node node(
			Node::Object_t::MODULE_VECTORSPACE_VECTOR, this,
			type, typeParam);

	Node::Id_t id = graph->AddNode(&node);
	if(Node::ID_NONE == id)
	{
		Error("Could not add node!\n");
		return false;
	}

	SetNodeRef(graph, id);

	return true;
}

VectorSpace::Vector::Vector(Graph* graph, const VectorSpace * vSpace)
{
	bool success = Init(
			graph,
			vSpace,
			nullptr,
			std::map<Property, const void *>{},
			Node::Type::VECTOR, nullptr);

	if(!success)
	{
		Error("Couldn't init vector!\n");
	}
}

VectorSpace::Vector::Vector(Graph* graph, const VectorSpace * vSpace, const std::map<Property, const void *> &properties)
{
	bool success = Init(
			graph,
			vSpace,
			nullptr,
			properties,
			Node::Type::VECTOR, nullptr);

	if(!success)
	{
		Error("Couldn't init vector!\n");
	}
}

VectorSpace::Vector::Vector(Graph* graph, const VectorSpace * vSpace, const void * value, const std::map<Vector::Property, const void *> &properties)
{
	bool success = Init(
			graph,
			vSpace,
			value,
			properties,
			Node::Type::VECTOR, nullptr);

	if(!success)
	{
		Error("Couldn't init vector!\n");
	}
}

VectorSpace::Vector::Vector(Graph* graph, const VectorSpace * vSpace, Node::Type type, void * typeParam)
{
	bool success = Init(
			graph,
			vSpace,
			nullptr,
			std::map<Property, const void *>{},
			type, typeParam);

	if(!success)
	{
		Error("Couldn't init vector!\n");
	}
}

const VectorSpace * VectorSpace::Vector::Space() const
{
	return Space_;
}

const void * VectorSpace::Vector::InitValue() const
{
	return Value_;
}

const VectorSpace::Vector* VectorSpace::Vector::Power(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors) const
{
	// TODO: Catch exponent of 0?
	if(GetGraph() != vec->GetGraph())
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	bool rArgScalar = (1 == vec->Space_->GetDim());
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
	if(Space_->Factors_.size() != 2 * lfactors.size())
	{
		Error("Can only take the power, if half of the indices are contracted!\n");
		return nullptr;
	}

	// Check if indices are inside allowed range
	for(size_t index = 0; index < lfactors.size(); index++)
	{
		if((Space_->Factors_.size() <= lfactors[index]) || (Space_->Factors_.size() <= rfactors[index]))
		{
			Error("At least one given contraction factor is larger than number of factors!\n");
			return nullptr;
		}

		if(Space_->Factors_[lfactors[index]].Dim != Space_->Factors_[rfactors[index]].Dim)
		{
			Error("At least one contraction index-pair has different dimension! Factor %u with %u, |lFactor| = %u, |rFactor| = %u\n",
					lfactors[index], rfactors[index],
					Space_->Factors_[lfactors[index]].Dim,
					vec->Space_->Factors_[rfactors[index]].Dim);

			return nullptr;
		}
	}

	// Check for dublicate indices
	if(hasDuplicates(lfactors) || hasDuplicates(rfactors))
	{
		Error("Dublicate contraction factors!\n");
		return nullptr;
	}


	Vector* retVec = new Vector(
			GetGraph(), Space_,
			Node::Type::VECTOR_POWER, nullptr);

	retVec->PushParent(Id());
	retVec->PushParent(vec->Id());

	return retVec;
}

template<typename inType>
const VectorSpace::Vector* VectorSpace::Vector::Power(inType exp) const
{
	// TODO: Catch exponent of 0?
	return Power(Space_->Scalar(GetGraph(), exp));
}

const VectorSpace::Vector* VectorSpace::Vector::Power(const Vector* vec) const
{
	// TODO: Catch exponent of 0?

	if(GetGraph() != vec->GetGraph())
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	if(1 < vec->Space_->GetDim())
	{
		Error("Can't take power to non-scalar value!\n");
		return nullptr;
	}

	Vector* retVec = new Vector(
			GetGraph(), Space_,
			Node::Type::VECTOR_POWER, nullptr);

	retVec->PushParent(Id());
	retVec->PushParent(vec->Id());

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::Divide(const Vector* vec) const
{
	const Vector * minusOne = vec->Space_->Scalar(vec->GetGraph(), -1.f);
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
	auto factorVec = Space_->Scalar(GetGraph(), factor);

	return Multiply(factorVec);
}

const VectorSpace::Vector* VectorSpace::Vector::Multiply(const Vector* vec) const
{
	if(GetGraph() != vec->GetGraph())
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	// Vector - Scalar multiplication: Do not add V-Space factors
	// Rationale for creating an exception by not adding factors: Multiplying scalars would quickly become a confusion of indices.
	bool lArgScalar = (1 == Space_->GetDim());
	bool rArgScalar = (1 == vec->Space_->GetDim());

	if(lArgScalar || rArgScalar)
	{
		// Infer space
		const VectorSpace * retSpace = nullptr;

		if(lArgScalar && rArgScalar)
		{
			Ring::type_t inferredRing = Ring::GetSuperiorRing(Space_->GetRing(), vec->Space_->GetRing());
			if(Ring::None == inferredRing)
			{
				Error("Incompatible Rings\n");
				return nullptr;
			}

			retSpace = new VectorSpace(inferredRing, 1);
		}
		else if(lArgScalar)
		{
			retSpace = vec->Space_;
		}
		else
		{
			retSpace = Space_;
		}

		if(nullptr == retSpace)
		{
			Error("Could not create VectorSpace");
			return nullptr;
		}

		Vector* retVec = new Vector(
				GetGraph(), retSpace,
				Node::Type::VECTOR_SCALAR_PRODUCT, nullptr);

		retVec->PushParent(Id());
		retVec->PushParent(vec->Id());

		return retVec;
	}

	VectorSpace * retSpace = nullptr;
	retSpace = new VectorSpace(std::initializer_list<const VectorSpace*>{Space_, vec->Space_});

	if(nullptr == retSpace)
	{
		Error("Could not create VectorSpace");
		return nullptr;
	}

	Vector* retVec = new Vector(
			GetGraph(), retSpace,
			Node::Type::VECTOR_VECTOR_PRODUCT, nullptr);

	retVec->PushParent(Id());
	retVec->PushParent(vec->Id());

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::JoinIndices(std::vector<std::vector<uint32_t>> &indices) const
{
	std::vector<uint32_t> indicesLinear;
	for(const auto &joinedIndices: indices)
	{
		for(const uint32_t &index: joinedIndices)
		{
			if(Space_->Factors_.size() <= index)
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
		const uint32_t indexDim = Space_->Factors_[joinedIndices[0]].Dim;
		for(const uint32_t &index: joinedIndices)
		{
			if(indexDim != Space_->Factors_[index].Dim)
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
	for(size_t factor = 0; factor < Space_->Factors_.size(); factor++)
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
			vsFactors.push_back(Space_->Factors_[factor]);
		}
	}

	VectorSpace * retSpace = new VectorSpace(vsFactors);

	Node::joinIndicesParameters_t * param = new Node::joinIndicesParameters_t;
	param->Indices = indices;
	std::sort(param->Indices.begin(), param->Indices.end());

	Vector* retVec = new Vector(
			GetGraph(), retSpace,
			Node::Type::VECTOR_JOIN_INDICES, param);

	retVec->PushParent(Id());

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::IsSmaller(const Vector* vec) const
{
	if(Space_->GetDim() != vec->Space_->GetDim())
	{
		Error("Dimension Mismatch!\n");
		return nullptr;
	}

	if(GetGraph() != vec->GetGraph())
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	Vector* retVec = new Vector(
			GetGraph(),new VectorSpace(Ring::Int32, 1),
			Node::Type::VECTOR_COMPARISON_IS_SMALLER, nullptr);

	retVec->PushParent(Id());
	retVec->PushParent(vec->Id());

	return retVec;
}

const VectorSpace::Vector * VectorSpace::Vector::MaxPool(const std::vector<uint32_t> &poolSize) const
{
	if(poolSize.size() != Space_->Factors_.size())
	{
		Error("Number of pool dimensions does not match number of argument factors!\n");
		return nullptr;
	}

	for(size_t factor = 0; factor < Space_->Factors_.size(); factor++)
	{
		if(poolSize[factor] > Space_->Factors_[factor].Dim)
		{
			Error("Pool dimension is larger than dimension of vector!\n");
			return nullptr;
		}

		if(Space_->Factors_[factor].Dim % poolSize[factor])
		{
			Error("Pool factor %lu does not divide factor %lu: %u vs. %u!\n",
					factor, factor,
					poolSize[factor],
					Space_->Factors_[factor].Dim);
			return nullptr;
		}
	}

	VectorSpace * retSpace = new VectorSpace(Space_->Factors_);

	// Pooling reduces the dimensions
	for(size_t factor = 0; factor < retSpace->Factors_.size(); factor++)
	{
		retSpace->Factors_[factor].Dim /= poolSize[factor];
	}

	auto param = new Node::PoolParameters_t;
	param->PoolSize = poolSize;

	Vector* retVec = new Vector(
			GetGraph(), retSpace,
			Node::Type::VECTOR_MAX_POOL, param);

	retVec->PushParent(Id());

	return retVec;
}

const VectorSpace::Vector * VectorSpace::Vector::CrossCorrelate(const Vector* Kernel) const
{
	if(GetGraph() != Kernel->GetGraph())
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	if(Space_->Factors_.size() != Kernel->Space_->Factors_.size())
	{
		Error("Different number of factors!\n");
		return nullptr;
	}

	for(size_t factor = 0; factor < Space_->Factors_.size(); factor++)
	{
		if(Space_->Factors_[factor].Dim < Kernel->Space_->Factors_[factor].Dim)
		{
			Error("Can't cross-correlate w.r.t. kernel-factor of smaller dimension than input!\n");
			return nullptr;
		}
	}

	for(const simpleVs_t &simpleVs: Kernel->Space_->Factors_)
	{
		if(2 > simpleVs.Dim)
		{
			Error("Can't cross-correlate w.r.t. kernel-factor of dimension less than 2!\n");
			return nullptr;
		}
	}

	// Results of cross-correlation is a vector with the same number of factors
	// However, the dimension of the factors decreases as
	// "the output is restricted to only positions where the kernel lies entirely within the image"
	// - "Deep Learning", Goodfellow et al.
	// This means if the Input (kernel) has dimension I (K), then the Output has dimension
	// O = I - (K - 1)
	VectorSpace * retSpace = new VectorSpace(Space_->Factors_);
	for(size_t factor = 0; factor < retSpace->Factors_.size(); factor++)
	{
		retSpace->Factors_[factor].Dim -= Kernel->Space_->Factors_[factor].Dim - 1;
	}

	Vector* retVec = new Vector(
			GetGraph(), retSpace,
			Node::Type::VECTOR_CROSS_CORRELATION, nullptr);

	retVec->PushParent(Id());
	retVec->PushParent(Kernel->Id());

	return retVec;
}

const std::map<VectorSpace::Vector::Property, const void *> * VectorSpace::Vector::Properties() const
{
	return &Properties_;
}

bool VectorSpace::Vector::AreCompatible(const Vector* vec1, const Vector* vec2)
{
	if(vec1->GetGraph() != vec2->GetGraph())
	{
		Error("Not on the same Graph!\n");
		return false;
	}

	if(vec1->Space_->Factors_.size() != vec2->Space_->Factors_.size())
	{
		Error("Product Space has different number of factors, %lu vs %lu!\n",
				vec1->Space_->Factors_.size(), vec2->Space_->Factors_.size());
		return false;
	}

	for(size_t factor = 0; factor < vec1->Space_->Factors_.size(); factor++)
	{
		if(vec1->Space_->Factors_[factor].Dim != vec2->Space_->Factors_[factor].Dim)
		{
			Error("Factor %lu is of different dimension!\n", factor);
			return false;
		}

		if(vec1->Space_->Factors_[factor].Ring != vec2->Space_->Factors_[factor].Ring)
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
	if(GetGraph() != vec->GetGraph())
	{
		Error("Not on the same Graph!\n");
		return nullptr;
	}

	// Find all operations where this node was effected by the input
	// Find this node
	const Node* thisNode = GetGraph()->GetNode(Id());
	if(nullptr == thisNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	// Go through all parents and their parents ... to see if input is involved
	// and build a graph for this
	std::map<Node::Id_t, depNode_t> dependenceGraph;
	TraverseParents(&dependenceGraph, Id(), vec->Id());

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
			if((0 == dep.second.parents.size()) && (vec->Id() != dep.first))
			{
				depNodesToRemove.push_back(dep.first);
			}
		}
	} while(depNodesToRemove.size());

	// The single root is the dependency node now.
#define PRINT_FINAL_DEP_TREE 1
#if PRINT_FINAL_DEP_TREE
	printf("dNode Id %u, Dep Node Id %u\n", Id(), vec->Id());
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

	return CreateDerivative(&dependenceGraph, this, vec->Id());
}

void VectorSpace::Vector::TraverseParents(std::map<Node::Id_t, depNode_t> * depNodes, Node::Id_t currentNode, Node::Id_t depNodeId) const
{
	const Node * currentNodePt = GetGraph()->GetNode(currentNode);
	if(nullptr == currentNodePt)
	{
		Error("Could not find node Id%u!\n", currentNode);
		return;
	}

	for(const auto &nextNodeId: *currentNodePt->Parents())
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
	if(currentVec->Id() == depNodeId)
	{
		Error("Taking derivative w.r.t. oneself!\n");
		return nullptr;
	}

	if(0 == (*depNodes)[currentVec->Id()].parents.size())
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
	for(const Node::Id_t &parentId: (*depNodes)[currentVec->Id()].parents)
	{
		const Node * parentNode = GetGraph()->GetNode(parentId);
		if(nullptr == parentNode)
		{
			Error("Unknown Node Id!\n");
			return nullptr;
		}

		const Node* fctNode = currentVec->GetGraph()->GetNode(currentVec->Id());
		printf("DepNode %u ParNode %u: Calculating derivative of %s (id %u) w.r.t. %s (id %u)\n",
				depNodeId, parentId,
				fctNode->getName(), fctNode->id,
				parentNode->getName(), parentNode->id);

		if(Node::Object_t::MODULE_VECTORSPACE_VECTOR != parentNode->GetObject())
		{
			Error("Can't take derivative w.r.t. non-vector node of object type %u!\n",
					(uint8_t) parentNode->GetObject());
			return nullptr;
		}

		// TODO: Check for Null returns
		const Vector * parentVec = (const Vector *) parentNode->GetObjectPt();

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

			if(1 != derivativeVec->Space_->GetDim()) // i.e. if a scalar function is derived w.r.t. vector argument
			{
				std::vector<uint32_t> lfactors(parentVec->Space_->Factors_.size());
				std::iota(lfactors.begin(), lfactors.end(),
						innerDerivative->Space_->Factors_.size() - parentVec->Space_->Factors_.size());

				std::vector<uint32_t> rfactors(parentVec->Space_->Factors_.size());
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
	const Node* fctNode = vecValuedFct->GetGraph()->GetNode(vecValuedFct->Id());
	if(fctNode->Parents()->end() == std::find(fctNode->Parents()->begin(), fctNode->Parents()->end(), arg->Id()))
	{
		Error("Tried to take derivative w.r.t. non-existing input node!\n");
		return nullptr;
	}

	switch(fctNode->GetType())
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

	case Node::Type::VECTOR_CROSS_CORRELATION:
		return CrossCorrelationDerivative(vecValuedFct, arg);

	default:
		Error("Node Type %s does not support taking its derivative!\n", Node::getName(fctNode->GetType()));
		return nullptr;
	}


	return nullptr;
}

const VectorSpace::Vector* VectorSpace::Vector::Contract(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors) const
{
	if(GetGraph() != vec->GetGraph())
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
		if((Space_->Factors_.size() <= lfactors[index]) || (vec->Space_->Factors_.size() <= rfactors[index]))
		{
			Error("At least one given contraction factor is larger than number of factors!\n");
			return nullptr;
		}

		if(Space_->Factors_[lfactors[index]].Dim != vec->Space_->Factors_[rfactors[index]].Dim)
		{
			Error("At least one contraction index-pair has different dimension! Factor %u with %u, |lFactor| = %u, |rFactor| = %u\n",
					lfactors[index], rfactors[index],
					Space_->Factors_[lfactors[index]].Dim,
					vec->Space_->Factors_[rfactors[index]].Dim);

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

	std::vector<simpleVs_t> lResidualFactors = Space_->Factors_;
	for(int factor = lfactorsSorted.size() - 1; factor >= 0; factor--)
	{
		lResidualFactors.erase(lResidualFactors.begin() + lfactorsSorted[factor]);
	}

	// Erase factors starting from the vector's tail
	std::vector<uint32_t> rfactorsSorted = rfactors;
	std::sort(rfactorsSorted.begin(), rfactorsSorted.end());

	std::vector<simpleVs_t> rResidualFactors = vec->Space_->Factors_;
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
				Space_->Factors_[0].Ring,
				vec->Space_->Factors_[0].Ring);

		factorsVec.push_back(simpleVs_t{superiorRing, 1});
	}

	VectorSpace * retSpace = nullptr;
	retSpace = new VectorSpace(factorsVec);
	if(nullptr == retSpace)
	{
		Error("Could not create VectorSpace");
		return nullptr;
	}

	Vector* retVec = nullptr;

	const Node * thisNode = GetGraph()->GetNode(Id());
	const Node * vecNode = GetGraph()->GetNode(vec->Id());

	if((Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT == thisNode->GetType()) &&
			(Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT == vecNode->GetType()))
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

		const Node::KroneckerDeltaParameters_t * thisKronParam = (const Node::KroneckerDeltaParameters_t *) thisNode->TypeParameters();
		const Node::KroneckerDeltaParameters_t * vecKronParam = (const Node::KroneckerDeltaParameters_t *) vecNode->TypeParameters();

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


			opKronParam->Scaling *= Space_->Factors_[lfactors[contrFactor]].Dim;
		}

		retVec = new Vector(
				GetGraph(), retSpace,
				Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT, opKronParam);
	}
	else
	{
		Node::contractParameters_t * opParameters = new Node::contractParameters_t;
		opParameters->lfactors = lfactors;
		opParameters->rfactors = rfactors;

		retVec = new Vector(
				GetGraph(), retSpace,
				Node::Type::VECTOR_CONTRACTION, opParameters);

		retVec->PushParent(Id());
		retVec->PushParent(vec->Id());
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
	if(1 != Space_->Factors_.size())
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
	if(range.size() != Space_->Factors_.size())
	{
		Error("Specify range for each factor!\n");
		return nullptr;
	}

	for(size_t factorRange = 0; factorRange < range.size(); factorRange++)
	{
		if((Space_->Factors_[factorRange].Dim < range[factorRange].first) ||
				(Space_->Factors_[factorRange].Dim < range[factorRange].second))
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

	std::vector<simpleVs_t> newFactors = Space_->Factors_;
	for(size_t factor = 0; factor < newFactors.size(); factor++)
	{
		newFactors[factor].Dim = range[factor].second - range[factor].first;
	}

	VectorSpace * retSpace = new VectorSpace(newFactors);

	Node::projectParameters_t * opParameters = new Node::projectParameters_t;
	opParameters->range = range;

	Vector* retVec = new Vector(
			GetGraph(), retSpace,
			Node::Type::VECTOR_PROJECTION, opParameters);

	retVec->PushParent(Id());

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
		if(index >= Space_->Factors_.size())
		{
			Error("Index is larger than number of factors!\n");
		}
	}

	if(indices.size() != Space_->Factors_.size())
	{
		Error("Number of permutation indices does not match number of factors!\n");
	}

	Node::permuteParameters_t * opParameters = new Node::permuteParameters_t;
	opParameters->indices = indices;

	Vector* retVec = new Vector(
			GetGraph(), Space_,
			Node::Type::VECTOR_PERMUTATION, opParameters);

	retVec->PushParent(Id());

	return retVec;
}

const VectorSpace::Vector * VectorSpace::Vector::IndexSplitSum(const std::vector<uint32_t> &splitPosition) const
{
	if(splitPosition.size() != Space_->Factors_.size())
	{
		Error("splitPosition dimension does not match number of factors!\n");
		return nullptr;
	}

	for(size_t factor = 0; factor < Space_->Factors_.size(); factor++)
	{
		if(splitPosition[factor] >= Space_->Factors_[factor].Dim - 1)
		{
			Error("Can't split factor of dimension %u at position %u!\n",
					Space_->Factors_[factor].Dim, splitPosition[factor]);
			return nullptr;
		}
	}

	std::vector<simpleVs_t> retSimpleVs;
	for(size_t factor = 0; factor < Space_->Factors_.size(); factor++)
	{
		retSimpleVs.push_back(Space_->Factors_[factor]);

		if(splitPosition[factor])
		{
			dimension_t originalDim = (retSimpleVs.end() - 1)->Dim;
			(retSimpleVs.end() - 1)->Dim = splitPosition[factor];

			retSimpleVs.push_back(Space_->Factors_[factor]);
			(retSimpleVs.end() - 1)->Dim = originalDim - splitPosition[factor] + 1;
		}
	}

	Node::splitSumIndicesParameters_t * opParameters = new Node::splitSumIndicesParameters_t;
	opParameters->SplitPosition = splitPosition;

	Vector* retVec = new Vector(
			GetGraph(), new VectorSpace(retSimpleVs),
			Node::Type::VECTOR_INDEX_SPLIT_SUM, opParameters);

	retVec->PushParent(Id());

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::CrossCorrelationDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	const Node * fctNode = vecValuedFct->GetGraph()->GetNode(vecValuedFct->Id());
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	bool argIsKernel = (arg->Id() == fctNode->Parents()->at(1));
	if(!argIsKernel)
	{
		Error("Not implemented: Cross-correlations derivative w.r.t. input\n"); // TODO: Implement
		return nullptr;
	}

	const Vector * kernelVector = arg; // for readability

	const Node * inputNode = vecValuedFct->GetGraph()->GetNode(fctNode->Parents()->at(0));
	if(nullptr == inputNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Vector * inputVector = (const Vector *) inputNode->GetObjectPt();

	// Derivative_klij = dOut(i,j) / dK(kl) = dI(i + m, j + n)K(m,n) / dK(kl) = I(i + k, j + l)
	std::vector<uint32_t> splitPos(kernelVector->Space_->Factors_.size());
	for(size_t factor = 0; factor < splitPos.size(); factor++)
	{
		splitPos[factor] = kernelVector->Space_->Factors_[factor].Dim;
	}

	const Vector * retVec = inputVector->IndexSplitSum(splitPos);
	if(nullptr == retVec)
	{
		Error("Could not IndexSplitSum!\n");
		return nullptr;
	}

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::ProjectDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	if(Ring::Float32 != arg->Space_->GetRing())
	{
		Error("Non implemented!\n");
		return nullptr;
	}

	const Node * fctNode = vecValuedFct->GetGraph()->GetNode(vecValuedFct->Id());
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Node::projectParameters_t * projParam = (const Node::projectParameters_t *) fctNode->TypeParameters();

	auto retSpace = new VectorSpace(std::vector<const VectorSpace*>{arg->Space_, vecValuedFct->Space_});

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
		std::vector<uint32_t> coord(retSpace->Factors_.size());
		coord[0] = index / strides[0];

		for(size_t factor = 1; factor < retSpace->Factors_.size(); factor++)
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
			arg->GetGraph(),
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

	const Node * fctNode = vecValuedFct->GetGraph()->GetNode(vecValuedFct->Id());
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Node * lNode = arg->GetGraph()->GetNode(fctNode->Parents()->at(0));
	if(nullptr == lNode)
	{
		Error("Could not find node Id%u!\n", fctNode->Parents()->at(0));
		return nullptr;
	}

	const Node * rNode = arg->GetGraph()->GetNode(fctNode->Parents()->at(1));
	if(nullptr == rNode)
	{
		Error("Could not find node Id%u!\n", fctNode->Parents()->at(1));
		return nullptr;
	}

	const Vector* baseVector = (const Vector *) lNode->GetObjectPt();
	const Vector* expVector = (const Vector *) rNode->GetObjectPt();

	bool derivativeWrtBase = (fctNode->Parents()->at(0) == arg->Id());

	if(!derivativeWrtBase)
	{
		Error("Taking derivative w.r.t. exponent is not implemented!\n");
		return nullptr;
	}

	// d/db b^e = e * b^(e-1)
	const Vector* minusOne = expVector->Space_->Scalar(expVector->GetGraph(), -1.f);
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
	if(1 == derivative->Space_->GetDim())
	{
		return derivative;
	}

	// d(a_ijk^2) / d(a_lmn) = 2 * delta_li * delta_mj * delta_nk * a_ijk (no sum)
	// Multiply with Kronecker
	std::vector<uint32_t> deltaPairs(2 * arg->Space_->Factors_.size());
	for(size_t deltaFactor = 0; deltaFactor < deltaPairs.size() / 2; deltaFactor++)
	{
		deltaPairs[deltaFactor] = deltaFactor + deltaPairs.size() / 2;
		deltaPairs[deltaFactor + deltaPairs.size() / 2] = deltaFactor;
	}

	VectorSpace * kronVectorSpace = new VectorSpace(*arg->Space_, 2);

	const Vector * kronVec = kronVectorSpace->Element(arg->GetGraph(), deltaPairs);
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
	for(size_t FacToJoin = 0; FacToJoin < arg->Space_->Factors_.size(); FacToJoin++)
	{
		indicesToJoin.push_back(
				std::vector<uint32_t>{
					(uint32_t) (arg->Space_->Factors_.size() + FacToJoin),
					(uint32_t) (2 * arg->Space_->Factors_.size() + FacToJoin)});
	}


	derivative = derivative->JoinIndices(indicesToJoin);

	return derivative;
}

const VectorSpace::Vector* VectorSpace::Vector::MultiplyDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	// Is arg the right side, i.e. the scalar?
	// On which side of the contraction is the non-arg node?
	const Node * fctNode = vecValuedFct->GetGraph()->GetNode(vecValuedFct->Id());
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	bool argOnRightSide;
	Node::Id_t otherNodeId;
	if(fctNode->Parents()->at(0) == arg->Id())
	{
		argOnRightSide = false;
		otherNodeId = fctNode->Parents()->at(1);
	}
	else
	{
		argOnRightSide = true;
		otherNodeId = fctNode->Parents()->at(0);
	}

	const Node * otherNode = arg->GetGraph()->GetNode(otherNodeId);
	if(nullptr == otherNode)
	{
		Error("Could not find node Id%u!\n", otherNodeId);
		return nullptr;
	}

	const Vector * otherVec = (const Vector *) otherNode->GetObjectPt();

	// Vector - Scalar multiplication: Do not add V-Space factors
	// Rationale for creating an exception by not adding factors: Multiplying scalars would quickly become a confusion of indices.
	bool argScalar = (1 == arg->Space_->GetDim());

	if(argScalar)
	{
		return otherVec;
	}

	std::vector<uint32_t> deltaPairs(2 * arg->Space_->Factors_.size());
	for(size_t deltaFactor = 0; deltaFactor < deltaPairs.size() / 2; deltaFactor++)
	{
		deltaPairs[deltaFactor] = deltaFactor + deltaPairs.size() / 2;
		deltaPairs[deltaFactor + deltaPairs.size() / 2] = deltaFactor;
	}

	VectorSpace * kronVectorSpace = new VectorSpace(*arg->Space_, 2);

	const Vector * kronVec = kronVectorSpace->Element(arg->GetGraph(), deltaPairs);
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
	std::vector<uint32_t> permutation(Product->Space_->Factors_.size());
	std::iota(permutation.begin(), permutation.end(), 0);

	// Move other factors to the left
	for(uint32_t otherFactor = 0; otherFactor < otherVec->Space_->Factors_.size(); otherFactor++)
	{
		permutation[arg->Space_->Factors_.size() + otherFactor] = arg->Space_->Factors_.size() + otherFactor + arg->Space_->Factors_.size();
	}

	// Move Arg factors to the right
	for(uint32_t argFactor = 0; argFactor < arg->Space_->Factors_.size(); argFactor++)
	{
		permutation[arg->Space_->Factors_.size() + otherVec->Space_->Factors_.size() + argFactor] = arg->Space_->Factors_.size() + argFactor;
	}

	return Product->Permute(permutation);
}

const VectorSpace::Vector* VectorSpace::Vector::PermuteDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	const Node * fctNode = vecValuedFct->GetGraph()->GetNode(vecValuedFct->Id());
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Node::permuteParameters_t * permutation = (const Node::permuteParameters_t *) fctNode->TypeParameters();

	// The result will just be a Kronecker product
	std::vector<uint32_t> deltaPairs(2 * arg->Space_->Factors_.size());
	for(size_t deltaFactor = 0; deltaFactor < deltaPairs.size() / 2; deltaFactor++)
	{
		deltaPairs[deltaFactor] = permutation->indices[deltaFactor] + deltaPairs.size() / 2;
		deltaPairs[permutation->indices[deltaFactor] + deltaPairs.size() / 2] = deltaFactor;
	}

	VectorSpace * kronVectorSpace = new VectorSpace(*arg->Space_, 2);

	return kronVectorSpace->Element(arg->GetGraph(), deltaPairs);
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
	const Node * fctNode = vecValuedFct->GetGraph()->GetNode(vecValuedFct->Id());
	if(nullptr == fctNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Node::contractParameters_t * contractValue = (const Node::contractParameters_t *) fctNode->TypeParameters();

	const std::vector<uint32_t> * argContrFactors;
	const std::vector<uint32_t> * otherContrFactors;
	const Node * otherNode = nullptr;
	bool argOnRightSide;
	if(fctNode->Parents()->at(0) == arg->Id())
	{
		argContrFactors = &contractValue->lfactors;
		otherContrFactors = &contractValue->rfactors;
		otherNode = vecValuedFct->GetGraph()->GetNode(fctNode->Parents()->at(1));
		argOnRightSide = false;
	}
	else
	{
		otherContrFactors = &contractValue->lfactors;
		argContrFactors = &contractValue->rfactors;
		otherNode = vecValuedFct->GetGraph()->GetNode(fctNode->Parents()->at(0));
		argOnRightSide = true;
	}

	if(nullptr == otherNode)
	{
		Error("Could not find node!\n");
		return nullptr;
	}

	const Vector * otherVec = (const Vector *) otherNode->GetObjectPt();

	// Create the Kronecker the otherVec (i.e. non-arg factor of contraction) will be contracted with
	// d/dB_lmn (A_opqr B_ops) = d(l,o) d(m,p) d(n,s) A_opqr
	std::vector<uint32_t> deltaPairs(2 * arg->Space_->Factors_.size());
	for(size_t deltaFactor = 0; deltaFactor < deltaPairs.size() / 2; deltaFactor++)
	{
		deltaPairs[deltaFactor] = deltaFactor + deltaPairs.size() / 2;
		deltaPairs[deltaFactor + deltaPairs.size() / 2] = deltaFactor;
	}

	VectorSpace * kronVectorSpace = new VectorSpace(*arg->Space_, 2);
	const Vector * kronVec = kronVectorSpace->Element(arg->GetGraph(), deltaPairs);

	std::vector<uint32_t> lFactors;
	lFactors.resize(otherContrFactors->size());

	for(size_t factor = 0; factor < lFactors.size(); factor++)
	{
		lFactors[factor] = argContrFactors->at(factor) + arg->Space_->Factors_.size();
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
		uint32_t nrOfUncontractedArgFactors = arg->Space_->Factors_.size() - argContrFactors->size();

		// Create Permutation:
		std::vector<uint32_t> permutation(returnVec->Space_->Factors_.size());
		std::iota(permutation.begin(), permutation.end(), 0);

		for(uint32_t uncontracted = 0; uncontracted < nrOfUncontractedArgFactors; uncontracted++)
		{
			permutation[arg->Space_->Factors_.size() + uncontracted] = permutation.size() - nrOfUncontractedArgFactors + uncontracted;
			permutation[permutation.size() - nrOfUncontractedArgFactors + uncontracted] = arg->Space_->Factors_.size() + uncontracted;
		}

		returnVec = returnVec->Permute(permutation);
	}

	return returnVec;
}

const VectorSpace::Vector* VectorSpace::Vector::AddDerivative(const Vector* vecValuedFct, const Vector* arg)
{
	// The new vector will be of tensor product vector space type.
	// The derivative vector's VS will come first (as in differential forms)
	std::vector<simpleVs_t> factors;
	factors.insert(factors.begin(), arg->Space_->Factors_.begin(), arg->Space_->Factors_.end());
	factors.insert(factors.end(), vecValuedFct->Space_->Factors_.begin(), vecValuedFct->Space_->Factors_.end());

	if(arg->Space_->Factors_.size() != vecValuedFct->Space_->Factors_.size())
	{
		Error("For addition the resulting tensor should have same dimensions as arguments!\n");
		return nullptr;
	}

	bool argIsScalar = (1 == arg->Space_->GetDim());
	if(argIsScalar)
	{
		// Do not add indices with Kronecker, but simply return identity.
		return vecValuedFct->Space_->Scalar(vecValuedFct->GetGraph(), 1.0f); // TODO: Only works for float
	}

	// General derivative of Add is easy: Just the product of Kronecker Deltas
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

	Vector* retVec = new Vector(
			vecValuedFct->GetGraph(), new VectorSpace(factors),
			Node::Type::VECTOR_KRONECKER_DELTA_PRODUCT, opParameters);

	return retVec;
}

const VectorSpace::Vector* VectorSpace::Vector::Subtract(const Vector* vec) const
{
	if(!AreCompatible(this, vec))
	{
		Error("Incompatible Vectors!\n");
		return nullptr;
	}

	const Vector * minusOne = vec->Space_->Scalar(vec->GetGraph(), -1.f);
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
	Ring::type_t inferredRing = Ring::GetSuperiorRing(Space_->GetRing(), vec->Space_->GetRing());
	if(Ring::None == inferredRing)
	{
		Error("Incompatible Rings\n");
		return nullptr;
	}

	const VectorSpace * retSpace;
	if(inferredRing == Space_->GetRing())
	{
		retSpace = Space_;
	}
	else
	{
		retSpace = vec->Space_;
	}

	Vector* retVec = new Vector(
			GetGraph(), retSpace,
			Node::Type::VECTOR_ADDITION, nullptr);

	retVec->PushParent(Id());
	retVec->PushParent(vec->Id());

	return retVec;
}

const std::vector<VectorSpace::simpleVs_t> * VectorSpace::Factors() const
{
	return &Factors_;
}

VectorSpace::VectorSpace(const std::vector<simpleVs_t> &factors)
{
	Factors_ = factors;
}

VectorSpace::VectorSpace(std::vector<const VectorSpace*> vSpaces)
{
	for(const VectorSpace * vSpace: vSpaces)
	{
		Factors_.insert(Factors_.end(), vSpace->Factors_.begin(), vSpace->Factors_.end());
	}
}

VectorSpace::VectorSpace(const VectorSpace &vSpace, size_t nTimes)
{
	while(nTimes--)
	{
		Factors_.insert(Factors_.end(), vSpace.Factors_.begin(), vSpace.Factors_.end());
	}
}

