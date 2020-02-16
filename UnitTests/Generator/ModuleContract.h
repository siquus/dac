/*
 * ModuleContract.h
 *
 *  Created on: Feb 9, 2020
 *      Author: derommo
 */

#ifndef UNITTESTS_MODULECONTRACT_H_
#define UNITTESTS_MODULECONTRACT_H_

#include "main.h"

class ModuleContract: public TestGenerator {
public:

	bool Generate(const std::string &path);
};



#endif /* UNITTESTS_MODULECONTRACT_H_ */
