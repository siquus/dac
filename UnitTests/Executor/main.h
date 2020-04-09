/*
 * main.h
 *
 *  Created on: Feb 16, 2020
 *      Author: derommo
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

		for(size_t row = 0; row < nData / (sizeof(float) * nCol); row++)
		{
			for(size_t col = 0; col < nCol; col++)
			{
				fprintf(stream, "%f, ", (double) data[col]);
			}
			fprintf(stream, "\n");
		}

		fprintf(stream, "\n");
		fprintf(stream, "\n");
	}

	int ErrorCnt_ = 0;
};



#endif /* UNITTESTS_PROGRAM_MAIN_H_ */
