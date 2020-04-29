/*
 * ModuleCNN.h
 *
 *  Created on: Apr 23, 2020
 *      Author: derommo
 */

#ifndef MODULECNN_H_
#define MODULECNN_H_

#include "main.h"

class ModuleCNN: public TestGenerator {
public:
	bool Generate(const std::string &path);
};

#endif /* MODULECNN_H_ */
