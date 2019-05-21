/*
 * GlobalDefines.h
 *
 *  Created on: May 20, 2019
 *      Author: derommo
 */

#ifndef SRC_GLOBALDEFINES_H_
#define SRC_GLOBALDEFINES_H_

#include <stdio.h>
#include <stdint.h>

#define Error(...) \
	fprintf(stderr, "File %s, Line %i: ", __FILE__, __LINE__); \
	fprintf(stderr, __VA_ARGS__)

typedef uint16_t dimension_t;



#endif /* SRC_GLOBALDEFINES_H_ */
