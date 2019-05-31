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
	outfile_ = fopen(path_.c_str(), "w");

	if(nullptr == outfile_)
	{
		Error("Open File %s failed: %s\n", path_.c_str(), strerror(errno));
		return false;
	}

	GenerateHeaderAndIncludes();

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
		int err = fprintf(outfile_, "%s\n", headerLines[line]);
		if(0 > err)
		{
			Error("fprintf on File %s failed: %s\n", path_.c_str(), strerror(errno));
			return false;
		}
	}

	for(uint16_t incl = 0; incl < sizeof(includeFilesBrackets) / sizeof(includeFilesBrackets[0]); incl++)
	{
		int err = fprintf(outfile_, "#include <%s>\n", includeFilesBrackets[incl]);
		if(0 > err)
		{
			Error("fprintf on File %s failed: %s\n", path_.c_str(), strerror(errno));
			return false;
		}
	}

	 for(uint16_t incl = 0; incl < sizeof(includeFilesQuotes) / sizeof(includeFilesQuotes[0]); incl++)
	 {
		 int err = fprintf(outfile_, "#include ''%s''\n", includeFilesQuotes[incl]);
		 if(0 > err)
		 {
			 Error("fprintf on File %s failed: %s\n", path_.c_str(), strerror(errno));
			 return false;
		 }
	 }

	return true;
}


