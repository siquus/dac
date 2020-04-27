#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <cerrno>
#include <time.h>
#include <math.h>

#include "common.h"

#include "DacSolarSystem.h"

#define fatal(...) \
	fprintf(stderr, "File %s, Line %i: ", __FILE__, __LINE__); \
	fprintf(stderr, __VA_ARGS__); \
	fflush(stderr); \
	exit(1)

// At 10000 iterations
static const float expectedTerminationState[2 * OBJECT_NROF * DIMENSIONS] = {
		5.436864122748374938965e-02, -2.608752995729446411133e-02, -1.282220706343650817871e-02,
		3.694848060607910156250e+00, -3.479873657226562500000e+00, -1.581821084022521972656e+00,
		6.928699016571044921875e+00, -6.636600017547607421875e+00, -3.039297819137573242188e+00,
		1.440823078155517578125e+01, 1.244436931610107421875e+01, 5.245813846588134765625e+00,
		2.969760894775390625000e+01, -3.484727621078491210938e+00, -2.166513442993164062500e+00,
		1.523834514617919921875e+01, -2.794991302490234375000e+01, -1.330021476745605468750e+01,
		1.808732434938065125607e-07, -8.569744750275276601315e-06, -3.668478257168317213655e-06,
		5.035052708990406244993e-06, 4.854637154494412243366e-06, 1.958277380254003219306e-06,
		1.065750780071539338678e-06, 1.020184527078527025878e-06, 3.754914246201224159449e-07,
		-1.189661276157494285144e-07, 1.065470485173136694357e-07, 4.834423705801782489289e-08,
		2.113485741972453979542e-08, 1.500443431723397225142e-07, 6.088395565484461258166e-08,
		2.230819522164306789591e-11, 7.749273445156745765416e-12, -4.292256220589374393626e-12
};

typedef struct {
	uint32_t WriteInterval = 1;
	std::string WritePath;
} cmdLineArgs_t;

typedef enum {
	CMD_LINE_OPTION_HELP,
	CMD_LINE_OPTION_WRITE_INTERVAL,
	CMD_LINE_OPTION_WRITE_PATH,
	CMD_LINE_OPTION_NROF,
} cmdLineOption_t;

typedef struct {
	char Option[5];
	char Param[4];
	char Name[100];
	char Help[100];
} cmdLineArgument_t;;

static const cmdLineArgument_t cmdLineArguments[CMD_LINE_OPTION_NROF] =
{
		{"-h", "", "Help", "Prints this help"},
		{"-i", "%u", "Interval", "[optional] Simulation step interval of logging the state"},
		{"-p", "%s", "Path", "[optional] Path to which the state will be written."}
};

typedef struct {
	FILE * File = nullptr;
	uint32_t Interval;
	uint32_t Cnt = 0;
} Output_t;

static Output_t Output;

static float LastState[2 * OBJECT_NROF * DIMENSIONS];

static void StateCallback(const float* pt, size_t size)
{
	memcpy(LastState, pt, sizeof(LastState));

	if(nullptr == Output.File)
	{
		return;
	}

	if(sizeof(LastState) != size)
	{
		fatal("Unexpected callback size: %lu vs %lu!\n",
				sizeof(LastState), size);
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

static void printHelp()
{
	printf("\n");
	for(int option = 0; option < CMD_LINE_OPTION_NROF; option++)
	{
		printf("%s\t %s\t %s: %s\n",
				cmdLineArguments[option].Option,
				cmdLineArguments[option].Param,
				cmdLineArguments[option].Name,
				cmdLineArguments[option].Help);
	}
	printf("\n");
}

static void handleCmdLineOption(cmdLineArgs_t * cmdLineArgs, cmdLineOption_t option, const char* arg)
{
	switch(option)
	{
	case CMD_LINE_OPTION_WRITE_INTERVAL:
	{
		errno = 0;
		char * tailptr;
		cmdLineArgs->WriteInterval = strtol(arg, &tailptr, 10);
		if(errno)
		{
			fatal("Could not convert \"%s\" to Number: %s!\n",
					arg,
					strerror(errno));
		}
		else if(arg == tailptr)
		{
			fatal("Could not convert \"%s\" to Number!\n", arg);
		}
		else if(0 == cmdLineArgs->WriteInterval)
		{
			fatal("0 is not a valid write interval!\n");
		}
	}
	break;


	case CMD_LINE_OPTION_WRITE_PATH:
		cmdLineArgs->WritePath = arg;
		break;

	default: // no break intended
	case CMD_LINE_OPTION_NROF:
		fatal("Unhandled option nr %u!\n", option);
	}
}

static void parseCmdLineArgs(cmdLineArgs_t * cmdLineArgs, int argc, char* argv[])
{
	for(int arg = 1; arg < argc; arg++)
	{
		bool foundOption = false;
		for(int option = 0; option < CMD_LINE_OPTION_NROF; option++)
		{
			if(0 == strncmp(cmdLineArguments[option].Option, argv[arg], sizeof(cmdLineArguments[option])))
			{
				foundOption = true;

				if(CMD_LINE_OPTION_HELP == option)
				{
					printHelp();
					exit(0);
				}

				if(arg + 1 >= argc)
				{
					printHelp();
					fatal("Missing parameter for %s\n", cmdLineArguments[option].Option);
				}

				arg++;

				handleCmdLineOption(cmdLineArgs, (cmdLineOption_t) option, argv[arg]);

				break;
			}
		}

		if(!foundOption)
		{
			printHelp();
			fatal("Unknown Option: %s\n", argv[arg]);
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

	// Check result
	for(size_t dim = 0; dim < sizeof(expectedTerminationState) / sizeof(expectedTerminationState[0]); dim++)
	{
		// We can't compare with constant tolerance, as we are comparing values in the range e-12 .. e+01
		// We can perform a relative comparison as we are not expecting a 0.f result.
		float relTolerance = FLT_EPSILON * fabsf(expectedTerminationState[dim]);

		if(fabsf(expectedTerminationState[dim] - LastState[dim]) > relTolerance)
		{
			fprintf(stderr, "Last state does not match expected!\n");
			exit(1);
		}
	}

	printf("DAC Runtime: %f Seconds\n",
			(dacEndClock - dacStartClock) / ((double) CLOCKS_PER_SEC));

	if(nullptr != Output.File)
	{
		fclose(Output.File);
	}

	return 0;
}
