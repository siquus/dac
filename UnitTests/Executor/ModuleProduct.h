/*
 * ModuleProduct.h
 *
 *  Created on: Mar 16, 2020
 *      Author: derommo
 */

#ifndef MODULEPRODUCT_H_
#define MODULEPRODUCT_H_

#include "main.h"

class ModuleProduct: public TestExecutor {
public:
	ModuleProduct();

	void Execute(size_t threadsNrOf);

	void VecScalarProduct(const float * data, size_t size);
	void ScalarVecProduct(const float * data, size_t size);
	void VecVecProduct(const float * data, size_t size);
	void KronVecProduct(const float * data, size_t size);
	void VecKronProduct(const float * data, size_t size);
	void DVecVecProductLeft(const float * data, size_t size);
	void DVecVecProductRight(const float * data, size_t size);

private:
	size_t ThreadsNrOf_ = 0;

	enum {
		CALLED_ScalarVecProduct,
		CALLED_VecScalarProduct,
		CALLED_VecVecProduct,
		CALLED_KronVecProduct,
		CALLED_VecKronProduct,
		CALLED_DVecVecProductLeft,
		CALLED_DVecVecProductRight,
		CALLED_NrOf,
	};

	bool called_[CALLED_NrOf] = {false};
};

#endif /* MODULEPRODUCT_H_ */
