/*
 * Ring.h
 *
 *  Created on: May 17, 2019
 *      Author: derommo
 */

#ifndef SRC_RING_H_
#define SRC_RING_H_

namespace Algebra {
namespace Ring {

enum class Type {
	None,
	Float32,
};

class Float32 {
};

extern Type GetSuperiorRing(Type t1, Type t2);

}
}
#endif /* SRC_RING_H_ */
