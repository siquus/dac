/*
 * embeddedFiles.cpp
 *
 *  Created on: Jul 21, 2019
 *      Author: derommo
 */

#include <string.h>

#include "GlobalDefines.h"
#include "embeddedFiles.h"

// Files saved with this binary.
// See https://csl.name/post/embedding-binary-data/
extern "C" {
extern const char _binary_______src_Helpers_c_start;
extern const char _binary_______src_Helpers_c_end;

extern const char _binary_______src_Helpers_h_start;
extern const char _binary_______src_Helpers_h_end;

extern const char _binary_______src_error_functions_c_start;
extern const char _binary_______src_error_functions_c_end;

extern const char _binary_______src_error_functions_h_start;
extern const char _binary_______src_error_functions_h_end;

extern const char _binary_______src_get_num_c_start;
extern const char _binary_______src_get_num_c_end;

extern const char _binary_______src_get_num_h_start;
extern const char _binary_______src_get_num_h_end;

extern const char _binary_______src_tlpi_hdr_h_start;
extern const char _binary_______src_tlpi_hdr_h_end;

extern const char _binary_______src_ename_c_inc_start;
extern const char _binary_______src_ename_c_inc_end;
}

typedef enum {
	EMBEDDED_FILES_HELPERS_C,
	EMBEDDED_FILES_HELPERS_H,
	EMBEDDED_FILES_ERROR_FUNCTIONS_C,
	EMBEDDED_FILES_ERROR_FUNCTIONS_H,
	EMBEDDED_FILES_GET_NUM_C,
	EMBEDDED_FILES_GET_NUM_H,
	EMBEDDED_FILES_TLPI_HDR_H,
	EMBEDDED_FILES_ENAME_C_INC,
	EMBEDDED_FILES_NROF,
} embeddedFiles_t;

typedef struct {
	const char* start;
	const char* end;
	const char name[42];
} embeddedFile_t;

const embeddedFile_t embeddedFiles[] = {
		[EMBEDDED_FILES_HELPERS_C] = {
				&_binary_______src_Helpers_c_start,
				&_binary_______src_Helpers_c_end,
				"Helpers.c"
		},
		[EMBEDDED_FILES_HELPERS_H] = {
				&_binary_______src_Helpers_h_start,
				&_binary_______src_Helpers_h_end,
				"Helpers.h"
		},
		[EMBEDDED_FILES_ERROR_FUNCTIONS_C] = {
				&_binary_______src_error_functions_c_start,
				&_binary_______src_error_functions_c_end,
				"error_functions.c"
		},
		[EMBEDDED_FILES_ERROR_FUNCTIONS_H] = {
				&_binary_______src_error_functions_h_start,
				&_binary_______src_error_functions_h_end,
				"error_functions.h"
		},
		[EMBEDDED_FILES_GET_NUM_C] = {
				&_binary_______src_get_num_c_start,
				&_binary_______src_get_num_c_end,
				"get_num.c"
		},
		[EMBEDDED_FILES_GET_NUM_H] = {
				&_binary_______src_get_num_h_start,
				&_binary_______src_get_num_h_end,
				"get_num.h"
		},
		[EMBEDDED_FILES_TLPI_HDR_H] = {
				&_binary_______src_tlpi_hdr_h_start,
				&_binary_______src_tlpi_hdr_h_end,
				"tlpi_hdr.h"
		},
		[EMBEDDED_FILES_ENAME_C_INC] = {
				&_binary_______src_ename_c_inc_start,
				&_binary_______src_ename_c_inc_end,
				"ename.c.inc"
		},
};

bool GenerateEmbeddedFiles(const std::string* path)
{
	for(uint8_t file = 0; file < sizeof(embeddedFiles) / sizeof(embeddedFiles[0]); file++)
	{
		std::string copyFilePath = *path + embeddedFiles[file].name;
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

