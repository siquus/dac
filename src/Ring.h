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

// Below Type ordering matters: Larger enum value means superior ring
typedef enum {
	None,
	Int32,
	Float32,
} type_t;

extern type_t GetSuperiorRing(type_t t1, type_t t2);

}
}
#endif /* SRC_RING_H_ */
