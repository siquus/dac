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

#include <string.h>
#include <unistd.h>

#include "GlobalDefines.h"
#include "embeddedFiles.h"

// Files saved with this binary.
// See https://csl.name/post/embedding-binary-data/
// TODO: The identifiers below depend on the path to that file.
extern "C" {
extern const char _binary_build_NodeExecutor_c_copy_start;
extern const char _binary_build_NodeExecutor_c_copy_end;

extern const char _binary_build_NodeExecutor_h_copy_start;
extern const char _binary_build_NodeExecutor_h_copy_end;

extern const char _binary_build_error_functions_c_copy_start;
extern const char _binary_build_error_functions_c_copy_end;

extern const char _binary_build_error_functions_h_copy_start;
extern const char _binary_build_error_functions_h_copy_end;

extern const char _binary_build_get_num_c_copy_start;
extern const char _binary_build_get_num_c_copy_end;

extern const char _binary_build_get_num_h_copy_start;
extern const char _binary_build_get_num_h_copy_end;

extern const char _binary_build_tlpi_hdr_h_copy_start;
extern const char _binary_build_tlpi_hdr_h_copy_end;

extern const char _binary_build_ename_c_inc_copy_start;
extern const char _binary_build_ename_c_inc_copy_end;
}

typedef enum {
	EMBEDDED_FILES_NodeExecutor_C,
	EMBEDDED_FILES_NodeExecutor_H,
	EMBEDDED_FILES_ERROR_FUNCTIONS_C,
	EMBEDDED_FILES_ERROR_FUNCTIONS_H,
	EMBEDDED_FILES_GET_NUM_C,
	EMBEDDED_FILES_GET_NUM_H,
	EMBEDDED_FILES_TLPI_HDR_H,
	EMBEDDED_FILES_ENAME_C_INC,
	EMBEDDED_FILES_NROF,
} t;

typedef struct {
	const char* start;
	const char* end;
	const char name[42];
} embeddedFile_t;

const embeddedFile_t embeddedFiles[] = {
		[EMBEDDED_FILES_NodeExecutor_C] = {
				&_binary_build_NodeExecutor_c_copy_start,
				&_binary_build_NodeExecutor_c_copy_end,
				"NodeExecutor.c"
		},
		[EMBEDDED_FILES_NodeExecutor_H] = {
				&_binary_build_NodeExecutor_h_copy_start,
				&_binary_build_NodeExecutor_h_copy_end,
				"NodeExecutor.h"
		},
		[EMBEDDED_FILES_ERROR_FUNCTIONS_C] = {
				&_binary_build_error_functions_c_copy_start,
				&_binary_build_error_functions_c_copy_end,
				"error_functions.c"
		},
		[EMBEDDED_FILES_ERROR_FUNCTIONS_H] = {
				&_binary_build_error_functions_h_copy_start,
				&_binary_build_error_functions_h_copy_end,
				"error_functions.h"
		},
		[EMBEDDED_FILES_GET_NUM_C] = {
				&_binary_build_get_num_c_copy_start,
				&_binary_build_get_num_c_copy_end,
				"get_num.c"
		},
		[EMBEDDED_FILES_GET_NUM_H] = {
				&_binary_build_get_num_h_copy_start,
				&_binary_build_get_num_h_copy_end,
				"get_num.h"
		},
		[EMBEDDED_FILES_TLPI_HDR_H] = {
				&_binary_build_tlpi_hdr_h_copy_start,
				&_binary_build_tlpi_hdr_h_copy_end,
				"tlpi_hdr.h"
		},
		[EMBEDDED_FILES_ENAME_C_INC] = {
				&_binary_build_ename_c_inc_copy_start,
				&_binary_build_ename_c_inc_copy_end,
				"ename.c.inc"
		},
};

bool GenerateEmbeddedFiles(const std::string* path)
{
	for(uint8_t file = 0; file < sizeof(embeddedFiles) / sizeof(embeddedFiles[0]); file++)
	{
		std::string copyFilePath = *path + embeddedFiles[file].name;

		// Has the file already been generated for another graph?
		if(0 == access(copyFilePath.c_str(), F_OK))
		{
#ifdef PREVENT_UNNECESSARY_COPYING // TODO: This is bothersome during development. Maybe compare timestamps or so? On the other hand this only saves ms
			break;
#endif // PREVENT_UNNECESSARY_COPYING
		}

		FILE* outFile = fopen(copyFilePath.c_str(), "w");
		if(nullptr == outFile)
		{
			Error("Open File %s failed: %s\n", copyFilePath.c_str(), strerror(errno));
			return false;
		}

		fwrite(embeddedFiles[file].start, sizeof(char),
				embeddedFiles[file].end - embeddedFiles[file].start,
				outFile);

		if(fclose(outFile))
		{
			Error("fclose failed: %s\n", strerror(errno));
			return false;
		}
	}


	return true;
}


