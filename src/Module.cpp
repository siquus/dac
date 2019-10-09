/*
 * Module.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#include <stdlib.h>
#include <cstring>
#include <type_traits>

#include "Module.h"

#include "GlobalDefines.h"

using namespace Algebra;
using namespace Module;

template VectorSpace::Vector * VectorSpace::Element<float>(Graph * graph, const std::vector<float>* initializer);

VectorSpace::VectorSpace(Ring::type_t ring, dimension_t dim)
{
	factors_.push_back(simpleVs_t{ring, dim});
}

dimension_t VectorSpace::GetDim() const
{
	dimension_t retDim = 0;
	for(const simpleVs_t &factor: factors_)
	{
		retDim += factor.dim_;
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

template<typename inType>
VectorSpace::Vector * VectorSpace::Element(Graph * graph, const std::vector<inType>* initializer)
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	if(initializer->size() != GetDim())
	{
		Error("Dimensions do not match!\n");
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

	retVec->__value_ = initializer->data();
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

