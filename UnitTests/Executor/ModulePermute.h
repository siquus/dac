/*
 * ModulePermute.h
 *
 *  Created on: Feb 29, 2020
 *      Author: derommo
 */

#ifndef MODULEPERMUTE_H_
#define MODULEPERMUTE_H_

#include "main.h"

class ModulePermute: public TestExecutor{
public:
	ModulePermute();

	void Execute(size_t threadsNrOf);
	void MatrixTranspose(const float * data, size_t size);
	void TensorPermute(const float * data, size_t size);

private:
	size_t ThreadsNrOf_ = 0;

	enum {
		CALLED_MatrixTranspose,
		CALLED_TensorPermute,
		CALLED_NrOf,
	};

	bool called_[CALLED_NrOf] = {false};
};

#endif /* MODULEPERMUTE_H_ */
