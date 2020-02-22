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
	void MatrixVecProd(const float * data, size_t size);
	void VecMatrixProd(const float * data, size_t size);
	void TensorVecContr2(const float * data, size_t size);
	void TensorVecContr1(const float * data, size_t size);

private:
	size_t ThreadsNrOf_ = 0;

	enum {
		CALLED_MatrixProd1,
		CALLED_MatrixVecProd,
		CALLED_VecMatrixProd,
		CALLED_TensorVecContr2,
		CALLED_TensorVecContr1,
		CALLED_NrOf,
	};

	bool called_[CALLED_NrOf] = {false};
};

#endif /* MODULECONTRACT_H_ */
