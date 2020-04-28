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

#ifndef MODULEPERMUTE_H_
#define MODULEPERMUTE_H_

#include "main.h"

class ModulePermute: public TestExecutor{
public:
	ModulePermute();

	void Execute(size_t threadsNrOf);
	void MatrixTranspose(const float * data, size_t size);
	void DMatrixTransposeContracted(const float * data, size_t size);
	void TensorPermute(const float * data, size_t size);
	void ProjVector(const float * data, size_t size);
	void DProjVector(const float * data, size_t size);

private:
	size_t ThreadsNrOf_ = 0;

	enum {
		CALLED_MatrixTranspose,
		CALLED_DMatrixTransposeContracted,
		CALLED_TensorPermute,
		CALLED_ProjVector,
		CALLED_DProjVector,
		CALLED_NrOf,
	};

	bool called_[CALLED_NrOf] = {false};
};

#endif /* MODULEPERMUTE_H_ */
