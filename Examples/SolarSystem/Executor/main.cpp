#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <cerrno>
#include <time.h>

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

// TODO: Create common header between generator and executor
#define DIMENSIONS 3u

typedef struct {
	char Name[10];
	float Mass; // [Sun], i.e. Mass(Sun) = 1
	float InitialPosition[DIMENSIONS];
	float InitialVelocity[DIMENSIONS];
} objectData_t;

typedef enum {
	OBJECT_SUN,
	OBJECT_JUPITER,
	OBJECT_SATURN,
	OBJECT_URANUS,
	OBJECT_NEPTUNE,
	OBJECT_PLUTO,
	OBJECT_NROF,
} object_t;

// See "Geometric Numerical Integration" p. 13ff for the data for "The Outer Solar System"
static const objectData_t Objects[OBJECT_NROF] = {
	[OBJECT_SUN] = {
			"Sun",
			1.00000597682,
			{0, 0, 0},
			{0, 0, 0}},
	[OBJECT_JUPITER] = {
			"Jupiter",
			0.000954786104043,
			{-3.5023653, -3.8169847, -1.5507963},
			{0.00565429, -0.00412490, -0.00190589}},
	[OBJECT_SATURN] = {
			"Saturn",
			0.000285583733151,
			{9.0755314, -3.0458353, -1.6483708},
			{0.00168318, 0.00483525, 0.00192462}},
	[OBJECT_URANUS] = {
			"Uranus",
			0.0000437273164546,
			{8.3101420, -16.2901086, -7.2521278},
			{0.00354178, 0.00137102, 0.00055029}},
	[OBJECT_NEPTUNE] = {
			"Neptune",
			0.0000517759138449,
			{11.4707666, -25.7294829, -10.8169456},
			{0.00288930, 0.00114527, 0.00039677}},
	[OBJECT_PLUTO] = {
			"Pluto",
			1.0 / 1.3E8,
			{-15.5387357, -25.2225594, -3.1902382},
			{0.00276725, -0.00170702, -0.00136504}}
};

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
