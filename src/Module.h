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
		Ring::type_t ring_; // TODO: Do we really want to implement different rings in one vector?
		dimension_t dim_;
	} simpleVs_t;

	VectorSpace(Ring::type_t ring, dimension_t dim);
	VectorSpace(Ring::type_t ring, const std::vector<dimension_t> &dimensions);
	VectorSpace(const std::vector<simpleVs_t> &factors); // TODO: Don'T expose simpleVs.
	VectorSpace(std::vector<const VectorSpace*> vSpaces);
	VectorSpace(const VectorSpace &vSpace, size_t nTimes);

	// Vector space created by the tensor product of given factors
	std::vector<simpleVs_t> factors_; // TODO: Currently not allowed to take the product of product spaces

	dimension_t GetDim() const;
	Ring::type_t GetRing() const;

	// Calculate Strides. // TODO: assumes Row-Major Layout
	// https://en.wikipedia.org/wiki/Row-_and_column-major_order#Address_calculation_in_general
	void GetStrides(std::vector<uint32_t> * strides) const;

	static bool AreEqual(const VectorSpace * lVs, const VectorSpace * rVs);

	class Vector : public NodeRef {
	public:
		const VectorSpace * __space_;
		const void* __value_ = nullptr;

		void PrintInfo() const;
		static bool SameValue(const Vector * lVec, const Vector * rVec);

		// TODO: Make these operators derived classes?
		// Then we don't have to weirdly hand over the argument order and stuff. They could carry a pointer to their derivative.
		const Vector* Add(const Vector* vec) const;
		const Vector* Subtract(const Vector* vec) const;

		template<typename inType>
		const Vector* Multiply(inType factor) const;
		const Vector* Multiply(const Vector* vec) const;
		const Vector* Divide(const Vector* vec) const;

		template<typename inType>
		const Vector* Power(inType exp) const; // element-wise power, e.g. c_ij^2 = c_ij * c_ij (no sum)
		const Vector* Power(const Vector* vec) const; // element-wise power, e.g. c_ij^2 = c_ij * c_ij (no sum)
		const Vector* Power(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors) const; // contraction power, e.g. B_ij^n = A_ij = B_ik B_kl B_lo ... n times

		const Vector* IsSmaller(const Vector* vec) const;

		const Vector* Contract(const Vector* vec, uint32_t lfactor = 0, uint32_t rfactor = 0) const;
		const Vector* Contract(const Vector* vec, const std::vector<uint32_t> &lfactors, const std::vector<uint32_t> &rfactors) const;

		const Vector* Permute(const std::vector<uint32_t> &indices) const; // I.e. the new vector is going to contain index indices[j] at position j.

		const Vector* Project(const std::vector<std::pair<uint32_t, uint32_t>> &range) const; // I.e. new, smaller vector has only the indices in range.
		const Vector* Project(const std::pair<uint32_t, uint32_t> &range) const; // I.e. new, smaller vector has only the indices in range.

		const Vector* JoinIndices(std::vector<std::vector<uint32_t>> &indices) const; // B_ik = JoinIndices(A_ijk, {0, 1})= A_iik (no sum)

		const Vector * CrossCorrelate(const Vector* Kernel) const; // TODO: Description. See "Deep learning", p.324
		const Vector * MaxPool(const std::vector<uint32_t> &poolSize) const; // https://en.wikipedia.org/wiki/Convolutional_neural_network#Pooling_layer

		const Vector * IndexSplitSum(const std::vector<uint32_t> &splitPosition) const; // IndexSplitSum(A_ij, {0, 3}) = B_ijk = A_i(j+k), where j = 0...2, k = 3..., i.e. splitPosition = 0 means this axis won't be split

		const Vector* Derivative(const Vector* vec) const;

		typedef struct {
			enum initializer_t {DENSE, COO, CSR, CSC};
			initializer_t Initializer;
			std::vector<uint32_t> Indices; // declaring which index is sparse. If size == 0 assume all are.
		} propertyParameterSparse_t;

		typedef struct {
			enum initializer_t {DENSE, LEFTMOST_INDICES}; // LEFTMOST_INDICES Only the leftmost of all symmetric indices supplied
			initializer_t Initializer;
			std::pair<uint32_t, uint32_t> Pairs; // declaring which two indices have this property. If size == 0 then complete symmetry.
		} propertyParameterSymmetric_t;

		typedef struct {
			enum initializer_t {DENSE, DIAGONAL}; // LEFTMOST_INDICES Only the leftmost of all symmetric indices supplied
			initializer_t Initializer;
			std::pair<uint32_t, uint32_t> Pairs; // declaring which two indices have this property. If size == 0, then all.
		} propertyParameterDiagonal_t;

		enum class Property {
			Diagonal, // propertyParameterDiagonal_t
			Symmetric, // propertyParameterSymmetric_t
			Antisymmetric, // propertyParameterSymmetric_t
			Sparse, // propertyParameterSparse_t
		};

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
		static const Vector* PowerDerivative(const Vector* vecValuedFct, const Vector* arg);
		static const Vector* ProjectDerivative(const Vector* vecValuedFct, const Vector* arg);
		static const Vector* CrossCorrelationDerivative(const Vector* vecValuedFct, const Vector* arg);
	};

	template<typename T>
	const Vector * Element(Graph* graph, const std::vector<T> &initializer) const; // initializer Pointer is taken

	template<typename T>
	const Vector * Element(
			Graph* graph,
			const std::vector<T> &initializer,
			Vector::Property property,
			const void * parameter = nullptr) const;

	template<typename T>
	const Vector * Element(
			Graph* graph,
			const std::vector<T> &initializer,
			const std::vector<Vector::Property> &properties,
			const std::vector<const void *> &propertiesParameter) const;  // initializer Pointer is taken

	const Vector * Element(Graph* graph, const std::vector<uint32_t> &DeltaPairs, float Scaling = 1.f) const;

	template<typename T>
	const Vector * Scalar(Graph* graph, const T &initializer) const; // Initializer is copied

	template<typename T>
	const Vector * Homomorphism(Graph* graph, const std::vector<T> &initializer) const;  // initializer Pointer is taken

	template<typename T>
	const Vector * Homomorphism(
			Graph* graph,
			const std::vector<T> &initializer,
			Vector::Property property,
			const void * parameter = nullptr) const;  // initializer Pointer is taken

	template<typename T>
	const Vector * Homomorphism(
			Graph* graph,
			const std::vector<T> &initializer,
			const std::vector<Vector::Property> &properties,
			const std::vector<const void *> &propertiesParameter) const;  // initializer Pointer is taken
};
}
}

#endif /* SRC_MODULE_H_ */
