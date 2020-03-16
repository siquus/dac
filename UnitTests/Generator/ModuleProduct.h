/*
 * ModuleProduct.h
 *
 *  Created on: Mar 16, 2020
 *      Author: derommo
 */

#ifndef MODULEPRODUCT_H_
#define MODULEPRODUCT_H_

#include "main.h"

class ModuleProduct: public TestGenerator {
public:
	bool Generate(const std::string &path);
};

#endif /* MODULEPRODUCT_H_ */
