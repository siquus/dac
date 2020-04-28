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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

#include "ControlTransfer.h"
#include "Graph.h"
#include "Module.h"
#include "Ring.h"
#include "Interface.h"
#include "CodeGenerator.h"

#include "ModuleContract.h"
#include "ModulePermute.h"
#include "ModuleProduct.h"

#include "main.h"

#define FATAL_ON_FALSE(arg) if(!arg){fatalLine(__FILE__, __LINE__, #arg);}

static void fatalLine(const char * file, int line, const char * lineString)
{
	fprintf(stderr, "%s:%i: Code generation failed for \"%s\"!\n",
			file, line, lineString);
	fflush(stderr);
	exit(1);
}

int main()
{
	struct stat stCodePath = {};
	const char path[] = "../Executor/dac";
	// Test if folder already exsists
	if (stat(path, &stCodePath) == -1)
	{
		mkdir(path, 0700);
	}

	auto outpath = std::string(path) + "/";

	ModuleContract moduleContract;
	FATAL_ON_FALSE(moduleContract.Generate(outpath));

	ModulePermute modulePermute;
	FATAL_ON_FALSE(modulePermute.Generate(outpath));

	ModuleProduct moduleProduct;
	FATAL_ON_FALSE(moduleProduct.Generate(outpath));

	printf("Success!\n");
	return 0;
}


