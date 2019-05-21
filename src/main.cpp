/*
 * main.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
 */

#include <stdio.h>

#include "Graph.h"
#include "Module.h"
#include "Ring.h"

int main()
{
	Graph graph;

	auto myVs = Algebra::Module::VectorSpace(Algebra::Ring::Float32(), 3);

	auto vec1_init = std::vector<float>{1, 2, 3};
	auto vec2_init = std::vector<float>{4, 5, 6};

	auto vec1 = myVs.Element(&graph, &vec1_init);

	auto vec2 = myVs.Element(&graph, &vec2_init);

	auto sum = vec1->Add(vec2);

	printf("Success!\n");
}


