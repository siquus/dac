/*
 * CodeGenerator.h
 *
 *  Created on: May 22, 2019
 *      Author: derommo
 */

#ifndef SRC_CODEGENERATOR_H_
#define SRC_CODEGENERATOR_H_

#include <string>
#include <fstream>

#include "Graph.h"

class CodeGenerator {
	std::string path_;
	FILE *outfile_ = nullptr;

	bool GenerateHeaderAndIncludes();

public:
	CodeGenerator(const std::string* path);
	virtual ~CodeGenerator();

	bool Generate(const Graph* graph);
};

#endif /* SRC_CODEGENERATOR_H_ */
