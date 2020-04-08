/*
 * common.h
 *
 *  Created on: Apr 8, 2020
 *      Author: derommo
 */


#ifndef COMMON_H_
#define COMMON_H_

#define DIMENSIONS 3u

typedef struct {
	char Name[10];
	float Mass; // [Sun], i.e. Mass(Sun) = 1
	float InitialPosition[DIMENSIONS];
	float InitialVelocity[DIMENSIONS];
} objectData_t;

typedef enum {
	OBJECT_SUN,
	OBJECT_JUPITER,
	OBJECT_SATURN,
	OBJECT_URANUS,
	OBJECT_NEPTUNE,
	OBJECT_PLUTO,
	OBJECT_NROF,
} object_t;

// See "Geometric Numerical Integration" p. 13ff for the data for "The Outer Solar System"
static const objectData_t Objects[OBJECT_NROF] = {
	[OBJECT_SUN] = {
			"Sun",
			1.00000597682,
			{0, 0, 0},
			{0, 0, 0}},
	[OBJECT_JUPITER] = {
			"Jupiter",
			0.000954786104043,
			{-3.5023653, -3.8169847, -1.5507963},
			{0.00565429, -0.00412490, -0.00190589}},
	[OBJECT_SATURN] = {
			"Saturn",
			0.000285583733151,
			{9.0755314, -3.0458353, -1.6483708},
			{0.00168318, 0.00483525, 0.00192462}},
	[OBJECT_URANUS] = {
			"Uranus",
			0.0000437273164546,
			{8.3101420, -16.2901086, -7.2521278},
			{0.00354178, 0.00137102, 0.00055029}},
	[OBJECT_NEPTUNE] = {
			"Neptune",
			0.0000517759138449,
			{11.4707666, -25.7294829, -10.8169456},
			{0.00288930, 0.00114527, 0.00039677}},
	[OBJECT_PLUTO] = {
			"Pluto",
			1.0 / 1.3E8,
			{-15.5387357, -25.2225594, -3.1902382},
			{0.00276725, -0.00170702, -0.00136504}}
};

#endif /* COMMON_H_ */
