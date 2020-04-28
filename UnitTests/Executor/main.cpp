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
