/*
 * This file is part of
 * Distributed Algebraic Computations (https://github.com/siquus/dac)
 *
 * GPL-3 (or later)
 *
 * Copyright (C) 2020  Patrik Omland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MODULEPRODUCT_H_
#define MODULEPRODUCT_H_

#include "main.h"

class ModuleProduct: public TestExecutor {
public:
	ModuleProduct();

	void Execute(size_t threadsNrOf);

	void ScalarScalarDiv(const float * data, size_t size);
	void DScalarScalarDivLeft(const float * data, size_t size);
	void DScalarScalarDivRight(const float * data, size_t size);
	void VecScalarProduct(const float * data, size_t size);
	void ScalarVecProduct(const float * data, size_t size);
	void VecVecProduct(const float * data, size_t size);
	void KronVecProduct(const float * data, size_t size);
	void VecKronProduct(const float * data, size_t size);
	void DVecScalarProdLeft(const float * data, size_t size);
	void DVecScalarProdRight(const float * data, size_t size);
	void DVecVecProductLeft(const float * data, size_t size);
	void DVecVecProductRight(const float * data, size_t size);
	void ScalarSquared(const float * data, size_t size);
	void DScalarSquaredBase(const float * data, size_t size);
	void VectorSquared(const float * data, size_t size);
	void DvectorSquaredBase(const float * data, size_t size);

private:
	size_t ThreadsNrOf_ = 0;

	enum {
		CALLED_ScalarScalarDiv,
		CALLED_DScalarScalarDivLeft,
		CALLED_DScalarScalarDivRight,
		CALLED_ScalarVecProduct,
		CALLED_VecScalarProduct,
		CALLED_VecVecProduct,
		CALLED_KronVecProduct,
		CALLED_VecKronProduct,
		CALLED_DVecScalarProdRight,
		CALLED_DVecScalarProdLeft,
		CALLED_DVecVecProductLeft,
		CALLED_DVecVecProductRight,
		CALLED_ScalarSquared,
		CALLED_DScalarSquaredBase,
		CALLED_VectorSquared,
		CALLED_DvectorSquaredBase,
		CALLED_NrOf,
	};

	bool called_[CALLED_NrOf] = {false};
};

#endif /* MODULEPRODUCT_H_ */
