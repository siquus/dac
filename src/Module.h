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
	VectorSpace(Ring::type_t ring, dimension_t dim);

	class Vector : public NodeRef {
	public:
		const VectorSpace* __space_;
		const void* __value_ = nullptr;

		Vector* Add(const Vector* vec);
		Vector* Multiply(const Vector* vec);
	};

	Ring::type_t ring_;
	dimension_t dim_;

	template<typename T>
	Vector * Element(Graph* graph, const std::vector<T>* initializer);
};

}
}

#endif /* SRC_MODULE_H_ */
