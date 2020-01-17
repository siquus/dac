/*
 * Module.h
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#ifndef SRC_MODULE_H_
#define SRC_MODULE_H_

#include <stdint.h>
#include <vector>
#include <map>
#include <set>

#include "GlobalDefines.h"
#include "Graph.h"
#include "Ring.h"


namespace Algebra {
namespace Module {

class VectorSpace {
public:
	typedef struct {
		Ring::type_t ring_;
		dimension_t dim_;
	} simpleVs_t;

	VectorSpace(Ring::type_t ring, dimension_t dim);
	VectorSpace(const std::vector<simpleVs_t>* factors); // TODO: Don'T expose simpleVs.
	VectorSpace(std::initializer_list<const VectorSpace*> list);

	// Vector space created by the tensor product of given factors
	std::vector<simpleVs_t> factors_; // TODO: Currently not allowed to take the product of product spaces

	dimension_t GetDim() const;
	Ring::type_t GetRing() const;
	void GetStrides(std::vector<uint32_t> * strides) const;

	class Vector : public NodeRef {
	public:
		const VectorSpace * __space_;
		const void* __value_ = nullptr;

		// TODO: Make these operators derived classes?
		// Then we don't have to weirdly hand over the argument order and stuff..
		Vector* Add(const Vector* vec);
		Vector* Multiply(const Vector* vec);
		Vector* IsSmaller(const Vector* vec);

		typedef struct {
			std::vector<uint32_t> lfactors;
			std::vector<uint32_t> rfactors;
		} contractValue_t;

		Vector* Contract(const Vector* vec, uint32_t lfactor = 0, uint32_t rfactor = 0);
		Vector* Contract(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors);

		Vector* Derivative(const Vector* vec);

		typedef struct {
			std::vector<uint32_t> factorPair; // I.e. for delta^i_j * delta^k_l * , ... position i will contain j and vice versa.
		} KroneckerDeltaParameters_t;

	private:
		static bool AreCompatible(const Vector* vec1, const Vector* vec2);

		typedef struct depNode_s {
			std::set<Node::Id_t> parents;
			std::set<Node::Id_t> children;
		} depNode_t;
		void TraverseParents(std::map<Node::Id_t, depNode_t> * depNodes, Node::Id_t currentNode, Node::Id_t depNodeId) const;
		VectorSpace::Vector* CreateDerivative(std::map<Node::Id_t, depNode_t> * depNodes, const VectorSpace::Vector * currentVec, Node::Id_t depNodeId);

		static VectorSpace::Vector* CreateDerivative(const Vector* vecValuedFct, const Vector* arg);
		static Vector* AddDerivative(const Vector* vecValuedFct, const Vector* arg);
	};

	template<typename T>
	Vector * Element(Graph* graph, const std::vector<T>* initializer);
};
}
}

#endif /* SRC_MODULE_H_ */
