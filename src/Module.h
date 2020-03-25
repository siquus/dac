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
#include <initializer_list>

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
	VectorSpace(const std::vector<simpleVs_t> &factors); // TODO: Don'T expose simpleVs.
	VectorSpace(std::initializer_list<const VectorSpace*> list);
	VectorSpace(const VectorSpace &vSpace, size_t nTimes);

	// Vector space created by the tensor product of given factors
	std::vector<simpleVs_t> factors_; // TODO: Currently not allowed to take the product of product spaces

	dimension_t GetDim() const;
	Ring::type_t GetRing() const;
	void GetStrides(std::vector<uint32_t> * strides) const;

	typedef struct {
		std::vector<uint32_t> DeltaPair; // I.e. for delta^i_j * delta^k_l * , ... position i will contain j and vice versa.
		float Scaling = 1.;
	} KroneckerDeltaParameters_t;

	class Vector : public NodeRef {
	public:
		const VectorSpace * __space_;
		const void* __value_ = nullptr;

		// TODO: Make these operators derived classes?
		// Then we don't have to weirdly hand over the argument order and stuff. They could carry a pointer to their derivative.
		const Vector* Add(const Vector* vec) const;
		const Vector* Multiply(const Vector* vec) const;
		const Vector* Divide(const Vector* vec) const;
		const Vector* Power(const Vector* vec) const; // element-wise power, e.g. c_ij^2 = c_ij * c_ij (no sum)
		const Vector* Power(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors) const; // contraction power, e.g. B_ij^n = A_ij = B_ik B_kl B_lo ... n times
		const Vector* IsSmaller(const Vector* vec) const;

		typedef struct {
			std::vector<uint32_t> lfactors;
			std::vector<uint32_t> rfactors;
		} contractValue_t;

		const Vector* Contract(const Vector* vec, uint32_t lfactor = 0, uint32_t rfactor = 0) const;
		const Vector* Contract(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors) const;

		typedef struct {
			std::vector<uint32_t> indices;
		} permuteParameters_t;
		const Vector* Permute(const std::vector<uint32_t> &indices) const; // I.e. the new vector is going to contain index indices[j] at position j.

		const Vector* Derivative(const Vector* vec) const;

	private:
		static bool AreCompatible(const Vector* vec1, const Vector* vec2);

		typedef struct depNode_s {
			std::set<Node::Id_t> parents;
			std::set<Node::Id_t> children;
		} depNode_t;
		void TraverseParents(std::map<Node::Id_t, depNode_t> * depNodes, Node::Id_t currentNode, Node::Id_t depNodeId) const;
		const VectorSpace::Vector* CreateDerivative(std::map<Node::Id_t, depNode_t> * depNodes, const VectorSpace::Vector * currentVec, Node::Id_t depNodeId) const;

		static const VectorSpace::Vector* CreateDerivative(const Vector* vecValuedFct, const Vector* arg);
		static const Vector* AddDerivative(const Vector* vecValuedFct, const Vector* arg);
		static const Vector* ContractDerivative(const Vector* vecValuedFct, const Vector* arg);
		static const Vector* PermuteDerivative(const Vector* vecValuedFct, const Vector* arg);
		static const Vector* MultiplyDerivative(const Vector* vecValuedFct, const Vector* arg);
		static const Vector* DivideDerivative(const Vector* vecValuedFct, const Vector* arg);
	};

	template<typename T>
	const Vector * Element(Graph* graph, const std::vector<T> &initializer) const; // initializer Pointer is taken

	const Vector * Element(Graph* graph, const KroneckerDeltaParameters_t &initializer) const; // initializer Pointer is taken

	template<typename T>
	const Vector * Scalar(Graph* graph, const T &initializer) const; // Initializer is copied

};
}
}

#endif /* SRC_MODULE_H_ */
