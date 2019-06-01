/*
 * CodeGenerator.cpp
 *
 *  Created on: May 22, 2019
 *      Author: derommo
 */

#include "CodeGenerator.h"

#include <iostream>
#include <string.h>
#include <stdint.h>

#include "GlobalDefines.h"
#include "Ring.h"

#define fprintProtect(ret) \
	{if(0 > ret) \
	{ \
		Error("fprintf on File %s failed: %s\n", path_.c_str(), strerror(errno)); \
		return false; \
	} \
	}

#define SNPRINTF(pt, size, ...) \
	{ \
		int ret = snprintf(pt, size, __VA_ARGS__); \
		if((0 > ret) || ((int) size <= ret)) \
		{ \
			Error("snprintf failed!\n"); \
			return false; \
		} \
	}


static const char includeFilesBrackets[][42] = {

};

static const char includeFilesQuotes[][42] = {

};

static const char headerLines[][100] = {
		"/*",
		" * Distributed Algebraic Computations",
		" * Copyright (C) 2019  Patrik Omland",
		" * This program is free software: you can redistribute it and/or modify",
		" * it under the terms of the GNU General Public License as published by",
		" * the Free Software Foundation, either version 3 of the License, or",
		" * (at your option) any later version.",
		" *",
		" * This program is distributed in the hope that it will be useful,",
		" * but WITHOUT ANY WARRANTY; without even the implied warranty of",
		" * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
		" * GNU General Public License for more details.",
		" *",
		" * You should have received a copy of the GNU General Public License",
		" * along with this program. If not, see <http://www.gnu.org/licenses/>.",
		" *",
		" * This program was generated by DAC ''Distributed Algebraic Computations''",
		" * The source code for the generator is available on GitHub",
		" * https://github.com/siquus/dac",
		" */",
};

CodeGenerator::CodeGenerator(const std::string* path) {
	path_ = *path;

}

CodeGenerator::~CodeGenerator() {
	fclose(outfile_);
}

bool CodeGenerator::Generate(const Graph* graph)
{
	graph_ = graph;
	outfile_ = fopen(path_.c_str(), "w");

	if(nullptr == outfile_)
	{
		Error("Open File %s failed: %s\n", path_.c_str(), strerror(errno));
		return false;
	}

	bool success;
	success = GenerateHeaderAndIncludes();
	if(!success)
	{
		Error("Could not generate Header");
		return false;
	}

	success = GenerateConstants();
	if(!success)
	{
		Error("Could not generate Constants");
		return false;
	}

	return true;
}

bool CodeGenerator::GenerateConstants()
{
	auto nodes = graph_->GetNodes();
	for(const Graph::Node_t &node: *nodes)
	{
		std::string declaration;
		switch(node.objectType)
		{
		case Graph::ObjectType::MODULE_VECTORSPACE_VECTOR:
		{
			auto vector = (const Algebra::Module::VectorSpace::Vector*) node.object;
			if(nullptr != vector->__value_)
			{
				declaration.append("static const ");
				bool success = GetVariableDeclaration(&declaration, vector);
				if(!success)
				{
					Error("Could not get Variable declaration");
					return false;
				}
			}
		}
			break;

		default: // no break intended
		case Graph::ObjectType::INTERFACE_OUTPUT:
			// No constants to allocate
			break;
		}

		if(!declaration.empty())
		{
			fprintProtect(fprintf(outfile_, "%s\n", declaration.c_str()));
		}
	}

	return true;
}

bool CodeGenerator::GetVariableDeclaration(std::string * declaration, const Algebra::Module::VectorSpace::Vector* vector)
{
	switch(vector->__space_->ring_)
	{
	case Algebra::Ring::Type::Float32:
	{
		char tmpBuffer[20];
		SNPRINTF(tmpBuffer, sizeof(tmpBuffer), "%s %s%u[%u] = {", "float", "node", vector->__nodeId_, vector->__space_->dim_);
		declaration->append(tmpBuffer);
		float * dataPt = (float*) vector->__value_;
		for(dimension_t dim = 0; dim < vector->__space_->dim_; dim++)
		{
			SNPRINTF(tmpBuffer, sizeof(tmpBuffer), "%f", dataPt[dim]);
			declaration->append(tmpBuffer);
			if(dim != vector->__space_->dim_ - 1)
			{
				declaration->append(", ");
			}
		}

		declaration->append("};");
	}
		break;

	case Algebra::Ring::Type::None: // no break intended
	default:
		Error("Unsupported Ring!\n");
		return false;
	}

	return true;
}

bool CodeGenerator::GenerateHeaderAndIncludes()
{
	if(nullptr == outfile_)
	{
		Error("Output file is not open!\n");
		return false;
	}

	for(uint16_t line = 0; line < sizeof(headerLines) / sizeof(headerLines[0]); line++)
	{
		fprintProtect(fprintf(outfile_, "%s\n", headerLines[line]));
	}

	for(uint16_t incl = 0; incl < sizeof(includeFilesBrackets) / sizeof(includeFilesBrackets[0]); incl++)
	{
		fprintProtect(fprintf(outfile_, "#include <%s>\n", includeFilesBrackets[incl]));
	}

	 for(uint16_t incl = 0; incl < sizeof(includeFilesQuotes) / sizeof(includeFilesQuotes[0]); incl++)
	 {
		 fprintProtect(fprintf(outfile_, "#include ''%s''\n", includeFilesQuotes[incl]));
	 }

	return true;
}


