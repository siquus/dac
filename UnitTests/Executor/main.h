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

	int ErrorCnt_ = 0;
};



#endif /* UNITTESTS_PROGRAM_MAIN_H_ */
