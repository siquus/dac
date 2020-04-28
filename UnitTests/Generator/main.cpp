/*
 * main.cpp
 *
 *  Created on: May 18, 2019
 *      Author: derommo
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


