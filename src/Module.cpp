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
	dim_ = dim;
	ring_ = ring;
}

template<typename inType>
VectorSpace::Vector * VectorSpace::Element(Graph * graph, const std::vector<inType>* initializer)
{
	if(nullptr == graph)
	{
		Error("nullptr\n");
		return nullptr;
	}

	if(initializer->size() != dim_)
	{
		Error("Dimensions do not match!\n");
		return nullptr;
	}

	Vector * retVec = new Vector;
	if(nullptr == retVec)
	{
		Error("Could not malloc Vec\n");
		return nullptr;
	}

	void * dataPt = nullptr;
	switch(ring_)
	{
	case Ring::Float32:
		if(!std::is_same<inType, float>::value)
		{
			Error("Type mismatch\n");
			return nullptr;
		}

		dataPt = malloc(dim_ * sizeof(float));
		if(nullptr != dataPt)
		{
			memcpy(dataPt, initializer->data(), dim_ * sizeof(float));
		}
		break;

	case Ring::Int32:
		if(!std::is_same<inType, int32_t>::value)
		{
			Error("Type mismatch\n");
			return nullptr;
		}

		dataPt = malloc(dim_ * sizeof(int32_t));
		if(nullptr != dataPt)
		{
			memcpy(dataPt, initializer->data(), dim_ * sizeof(int32_t));
		}
		break;

	default:
		Error("Type mismatch\n");
		return nullptr;
	}

	if(nullptr == dataPt)
	{
		Error("Malloc failed!\n");
		return nullptr;
	}

	Node node;
	node.Type = Node::Type::VECTOR;
	node.objectType = Node::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	Node::Id_t nodeId = graph->AddNode(&node);
	if(Node::ID_NONE == nodeId)
	{
		Error("Could not add node!\n");
		return nullptr;
	}

	retVec->__value_ = dataPt;
	retVec->__space_ = this;
	retVec->graph_ = graph;
	retVec->nodeId_ = nodeId;

	return retVec;
}

VectorSpace::Vector* VectorSpace::Vector::Multiply(const Vector* vec)
{
	if(1 != vec->__space_->dim_)
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
	Ring::type_t inferredRing = Ring::GetSuperiorRing(__space_->ring_, vec->__space_->ring_);
	if(Ring::None == inferredRing)
	{
		Error("Incompatible Rings\n");
		return nullptr;
	}

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;

	VectorSpace * retSpace = nullptr;
	retSpace = new VectorSpace(inferredRing, __space_->dim_);

	if(nullptr == retSpace)
	{
		Error("Could not create VectorSpace");
		return nullptr;
	}

	retVec->__space_ = retSpace;

	Node node;
	node.parents.push_back(nodeId_);
	node.parents.push_back(vec->nodeId_);
	node.Type = Node::Type::VECTOR_SCALAR_MULTIPLICATION;
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

VectorSpace::Vector* VectorSpace::Vector::Add(const Vector* vec)
{
	if(__space_->dim_ != vec->__space_->dim_)
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
	Ring::type_t inferredRing = Ring::GetSuperiorRing(__space_->ring_, vec->__space_->ring_);
	if(Ring::None == inferredRing)
	{
		Error("Incompatible Rings\n");
		return nullptr;
	}

	Vector* retVec = new Vector;
	retVec->graph_ = graph_;

	if(inferredRing == __space_->ring_)
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
	node.Type = Node::Type::VECTOR_ADDITION;
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
