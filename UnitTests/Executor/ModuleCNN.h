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

#ifndef MODULECNN_H_
#define MODULECNN_H_

#include "main.h"

class ModuleCNN: public TestExecutor {
public:
	ModuleCNN();

	void Execute(size_t threadsNrOf);

	void CC(const float * data, size_t size);
	void CCMaxPool(const float * data, size_t size);

private:
	size_t ThreadsNrOf_ = 0;

	enum {
		CALLED_CC,
		CALLED_CCMaxPool,
		CALLED_NrOf,
	};

	bool called_[CALLED_NrOf] = {false};
};


#endif /* MODULECNN_H_ */
