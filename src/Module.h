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
	VectorSpace(const std::vector<simpleVs_t>* factors);

	// Vector space created by the tensor product of given factors
	std::vector<simpleVs_t> factors_; // TODO: Currently not allowed to take the product of product spaces

	dimension_t GetDim() const;
	Ring::type_t GetRing() const;
	void GetStrides(std::vector<uint32_t> * strides) const;

	class Vector : public NodeRef {
	public:
		const VectorSpace * __space_;
		const void* __value_ = nullptr;
		const void* __operationParameters_ = nullptr;

		// TODO: Make these operators derived classes?
		// Then we don't have to weirdly hand over the argument order and stuff..
		Vector* Add(const Vector* vec);
		Vector* Multiply(const Vector* vec);
		Vector* IsSmaller(const Vector* vec);

		typedef struct {
			uint32_t lfactor;
			uint32_t rfactor;
		} contractValue_t;

		Vector* Contract(const Vector* vec, uint32_t lfactor = 0, uint32_t rfactor = 0);
	private:
		static bool AreCompatible(const Vector* vec1, const Vector* vec2);
	};

	template<typename T>
	Vector * Element(Graph* graph, const std::vector<T>* initializer);
};
}
}

#endif /* SRC_MODULE_H_ */
