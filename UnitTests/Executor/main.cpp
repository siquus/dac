#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "error_functions.h"

#include "ModuleContract.h"
#include "ModulePermute.h"
#include "ModuleProduct.h"


int main() {

	ModuleContract moduleContract;
	moduleContract.Execute(4);
	if(!moduleContract.Success())
	{
		fatal("Not all tests passed!\n");
	}

	ModulePermute modulePermute;
	modulePermute.Execute(4);
	if(!modulePermute.Success())
	{
		fatal("Not all tests passed!\n");
	}

	ModuleProduct moduleProduct;
	moduleProduct.Execute(4);
	if(!moduleProduct.Success())
	{
		fatal("Not all tests passed!\n");
	}

	fprintf(stdout, "SUCCESS!!\n");
	fflush(stdout);

	return 0;
}
