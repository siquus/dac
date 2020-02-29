/*
 * ModulePermute.h
 *
 *  Created on: Feb 29, 2020
 *      Author: derommo
 */

#ifndef MODULEPERMUTE_H_
#define MODULEPERMUTE_H_

#include "main.h"

class ModulePermute: public TestGenerator {
public:
	bool Generate(const std::string &path);
};

#endif /* MODULEPERMUTE_H_ */
