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

#ifndef SRC_GLOBALDEFINES_H_
#define SRC_GLOBALDEFINES_H_

#include <stdio.h>
#include <stdint.h>

#define Fatal(...) \
	fprintf(stderr, "File %s, Line %i: ", __FILE__, __LINE__); \
	fprintf(stderr, __VA_ARGS__); \
	fflush(stderr); \
	exit(1)

#define Error(...) \
	fprintf(stderr, "File %s, Line %i: ", __FILE__, __LINE__); \
	fprintf(stderr, __VA_ARGS__); \
	fflush(stderr)

#define ErrorContinued(...) \
	fprintf(stderr, __VA_ARGS__)

#define DEBUG(...) \
	printf(__VA_ARGS__); \
	fflush(stdout)

typedef uint32_t dimension_t;



#endif /* SRC_GLOBALDEFINES_H_ */
