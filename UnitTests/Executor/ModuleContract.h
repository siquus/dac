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
	void TensorMatrixContr1(const float * data, size_t size);
	void TensorMatrixContr12(const float * data, size_t size);
	void MatrixIdProd(const float * data, size_t size);
	void TwoMatrixTrace(const float * data, size_t size);
	void MatrixProdRight(const float * data, size_t size);
	void MatrixProdLeft(const float * data, size_t size);

private:
	size_t ThreadsNrOf_ = 0;

	enum {
		CALLED_MatrixProd1,
		CALLED_MatrixVecProd,
		CALLED_VecMatrixProd,
		CALLED_TensorVecContr2,
		CALLED_TensorVecContr1,
		CALLED_TensorMatrixContr1,
		CALLED_TensorMatrixContr12,
		CALLED_MatrixIdProd,
		CALLED_TwoMatrixTrace,
		CALLED_MatrixProdRight,
		CALLED_MatrixProdLeft,
		CALLED_NrOf,
	};

	bool called_[CALLED_NrOf] = {false};
};

#endif /* MODULECONTRACT_H_ */
