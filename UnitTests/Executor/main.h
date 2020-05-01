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

#ifndef UNITTESTS_PROGRAM_MAIN_H_
#define UNITTESTS_PROGRAM_MAIN_H_

#include <stdio.h>

#define Error(...) \
	fprintf(stderr, "\nERROR: %s:%i %s: ", __FILE__, __LINE__, __PRETTY_FUNCTION__); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	fflush(stderr); \
	ErrorCnt_++;


class TestExecutor {
public:
	virtual ~TestExecutor(){};

	virtual void Execute(size_t threadsNrOf) = 0;
	bool Success() {return ErrorCnt_?false:true;};

	// row major!
	void PrintMatrix(FILE *stream, const float * data, size_t nData, size_t nCol)
	{
		if((0 == nCol) || (0 == nData))
		{
			Error("Can't print matrix: nCol or nData == 0\n");
			return;
		}

		if(0 != (nData % (sizeof(float) * nCol)))
		{
			Error("Can't print matrix: Nr of columns incompatible with size!\n");
			return;
		}

		const size_t nRow = nData / (sizeof(float) * nCol);
		for(size_t row = 0; row < nRow; row++)
		{
			for(size_t col = 0; col < nCol; col++)
			{
				fprintf(stream, "%f, ", (double) data[row * nCol + col]);
			}
			fprintf(stream, "\n");
		}

		fprintf(stream, "\n");
		fprintf(stream, "\n");
	}

	int ErrorCnt_ = 0;
};



#endif /* UNITTESTS_PROGRAM_MAIN_H_ */
