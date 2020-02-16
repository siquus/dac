/*
 * ModuleContract.h
 *
 *  Created on: Feb 16, 2020
 *      Author: derommo
 */

#ifndef MODULECONTRACT_H_
#define MODULECONTRACT_H_

#include "main.h"

class ModuleContract: public TestExecutor {
public:
	ModuleContract();

	void Execute(size_t threadsNrOf);

	void MatrixProd1(const float * data, size_t size);

private:
	size_t ThreadsNrOf_ = 0;

	bool MatrixProd1Called_ = false;
};

#endif /* MODULECONTRACT_H_ */
