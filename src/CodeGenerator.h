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
#include "Module.h"

class CodeGenerator {
	std::string path_;
	FILE *outfile_ = nullptr;
	const Graph* graph_ = nullptr;

	bool GenerateConstants();
	bool GenerateStatics();
	bool GenerateHeaderAndIncludes();
	bool GenerateHeading(const std::string * heading);
	bool GetVariableDeclaration(std::string * declaration, const Algebra::Module::VectorSpace::Vector* vector, const std::string * prefix = nullptr);

public:
	CodeGenerator(const std::string* path);
	virtual ~CodeGenerator();

	bool Generate(const Graph* graph);
};

#endif /* SRC_CODEGENERATOR_H_ */
