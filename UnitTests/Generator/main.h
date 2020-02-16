/*
 * main.h
 *
 *  Created on: Feb 9, 2020
 *      Author: derommo
 */

#ifndef UNITTESTS_MAIN_H_
#define UNITTESTS_MAIN_H_

#include <string>

class TestGenerator {
public:
	virtual ~TestGenerator(){};

	virtual bool Generate(const std::string &path) = 0;
};


#endif /* UNITTESTS_MAIN_H_ */
