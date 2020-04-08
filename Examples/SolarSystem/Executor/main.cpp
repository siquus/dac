#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <cerrno>
#include <time.h>

#include "common.h"

#include "DacSolarSystem.h"

#define fatal(...) \
	fprintf(stderr, "File %s, Line %i: ", __FILE__, __LINE__); \
	fprintf(stderr, __VA_ARGS__); \
	fflush(stderr); \
	exit(1)

typedef struct {
	uint32_t WriteInterval = 1;
	std::string WritePath;
} cmdLineArgs_t;

typedef struct {
	FILE * File = nullptr;
	uint32_t Interval;
	uint32_t Cnt = 0;
} Output_t;

static Output_t Output;

static void StateCallback(const float* pt, size_t size)
{
	if(nullptr == Output.File)
	{
		return;
	}

	const size_t expectedSize = 2 * OBJECT_NROF * DIMENSIONS * sizeof(float);
	if(expectedSize != size)
	{
		fatal("Unexpected callback size: %lu vs %lu!\n",
				expectedSize, size);
	}

	Output.Cnt++;
	if(0 == (Output.Cnt % Output.Interval))
	{
		for(uint32_t object = 0; object < OBJECT_NROF; object++)
		{
			const size_t objectStartIndex = object * DIMENSIONS;

			int printRet = fprintf(Output.File, "%.*e, %.*e, %.*e,",
					 DECIMAL_DIG, (double) pt[objectStartIndex],
					 DECIMAL_DIG, (double) pt[objectStartIndex + 1],
					 DECIMAL_DIG, (double) pt[objectStartIndex + 2]);

			if(0 > printRet)
			{
				fatal("Printing to file failed: %s!\n", strerror(errno));
			}
		}

		int printRet = fprintf(Output.File, "\n");
		if(0 > printRet)
		{
			fatal("Printing to file failed: %s!\n", strerror(errno));
		}
	}
}

static void parseCmdLineArgs(cmdLineArgs_t * cmdLineArgs, int argc, char* argv[])
{
	for(int arg = 1; arg < argc; arg++)
	{
		if(arg + 1 >= argc)
		{
			fatal("Every command line argument has the format -%%c [input]\n");
		}
		else if(0 == memcmp("-p", argv[arg], sizeof("-p")))
		{
			arg++;
			cmdLineArgs->WritePath = argv[arg];
		}
		else if(0 == memcmp("-i", argv[arg], sizeof("-i")))
		{
			arg++;

			errno = 0;
			char * tailptr;
			cmdLineArgs->WriteInterval = strtol(argv[arg], &tailptr, 10);
			if(errno)
			{
				fatal("Could not convert \"%s\" to Number: %s!\n",
						argv[arg],
						strerror(errno));
			}
			else if(argv[arg] == tailptr)
			{
				fatal("Could not convert \"%s\" to Number!\n", argv[arg]);
			}
			else if(0 == cmdLineArgs->WriteInterval)
			{
				fatal("0 is not a valid write interval!\n");
			}
		}
		else
		{
			fatal("Unknown cmd line argument \"%s\"!\n", argv[arg]);
		}
	}
}

int main(int argc, char* argv[])
{
	cmdLineArgs_t cmdLineArgs;
	parseCmdLineArgs(&cmdLineArgs, argc, argv);

	// Create file for output?
	if(cmdLineArgs.WritePath.size())
	{
		Output.Interval = cmdLineArgs.WriteInterval;

		Output.File = fopen(cmdLineArgs.WritePath.c_str(), "w");
		if(nullptr == Output.File)
		{
			fatal("Open File %s failed: %s\n", cmdLineArgs.WritePath.c_str(), strerror(errno));
		}

		// Create csv header
		for(uint32_t object = 0; object < OBJECT_NROF; object++)
		{
			int printRet = fprintf(Output.File, "%s, %s, %s, ",
					Objects[object].Name, Objects[object].Name, Objects[object].Name);

			if(0 > printRet)
			{
				fatal("Printing to file failed: %s!\n", strerror(errno));
			}
		}
		int printRet = fprintf(Output.File, "\n");
		if(0 > printRet)
		{
			fatal("Printing to file failed: %s!\n", strerror(errno));
		}
	}

	DacSolarSystemOutputCallbackNewState_Register(&StateCallback);

	clock_t dacStartClock = clock();
	DacSolarSystemRun(4);
	clock_t dacEndClock = clock();

	printf("DAC Runtime: %f Seconds\n",
			(dacEndClock - dacStartClock) / ((double) CLOCKS_PER_SEC));

	if(nullptr != Output.File)
	{
		fclose(Output.File);
	}

	return 0;
}
