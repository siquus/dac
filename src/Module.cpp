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

VectorSpace::VectorSpace(Ring::Float32 f32, dimension_t dim)
{
	dim_ = dim;
	ring_ = Ring::Type::Float32;
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

	// TODO: Template initializer functions rather than type-switch?
	// https://isocpp.org/wiki/faq/templates#template-specialization-piecemeal
	void * dataPt = nullptr;
	switch(ring_)
	{
	case Ring::Type::Float32:
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

	default:
		Error("Type mismatch\n");
		return nullptr;
	}

	if(nullptr == dataPt)
	{
		Error("Malloc failed!\n");
		return nullptr;
	}

	Graph::Node_t node;
	node.nodeType = Graph::NodeType::VECTOR;
	node.objectType = Graph::ObjectType::MODULE_VECTORSPACE_VECTOR;
	node.object = retVec;

	Graph::NodeId_t nodeId = graph->AddNode(&node);
	if(Graph::NODE_ID_NONE == nodeId)
	{
		Error("Could not add node!");
		return nullptr;
	}

	retVec->__value_ = dataPt;
	retVec->__space_ = this;
	retVec->__graph_ = graph;
	retVec->__nodeId_ = nodeId;

	return retVec;
}

VectorSpace::Vector* VectorSpace::Vector::Add(const Vector* vec)
{
	if(__space_->dim_ != vec->__space_->dim_)
	{
		Error("Dimension Mismatch!");
		return nullptr;
	}

	if(__graph_ != vec->__graph_)
	{
		Error("Not on the same Graph!");
		return nullptr;
	}

	// Infer space
	Ring::Type inferredRing = Ring::GetSuperiorRing(__space_->ring_, vec->__space_->ring_);
	if(Ring::Type::None == inferredRing)
	{
		Error("Incompatible Rings");
		return nullptr;
	}

	Vector* retVec = new Vector;
	retVec->__graph_ = __graph_;

	if(inferredRing == __space_->ring_)
	{
		retVec->__space_ = __space_;
	}
	else
	{
		retVec->__space_ = vec->__space_;
	}

	Graph::Node_t node;
	node.parents.push_back(__nodeId_);
	node.parents.push_back(vec->__nodeId_);
	node.nodeType = Graph::NodeType::VECTOR_ADDITION;
	node.objectType = Graph::ObjectType::MODULE_VECTORSPACE_VECTOR;

	Graph::NodeId_t addNodeId = __graph_->AddNode(&node);

	if(Graph::NODE_ID_NONE == addNodeId)
	{
		Error("Could not add Node!\n");
		return nullptr;
	}

	return retVec;
}
