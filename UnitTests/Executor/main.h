/*
 * main.h
 *
 *  Created on: Feb 16, 2020
 *      Author: derommo
 */

#ifndef UNITTESTS_PROGRAM_MAIN_H_
#define UNITTESTS_PROGRAM_MAIN_H_

class TestExecutor {
	virtual ~TestExecutor(){};

	virtual bool Execute() = 0;
};



#endif /* UNITTESTS_PROGRAM_MAIN_H_ */
