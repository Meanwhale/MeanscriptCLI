#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//#include "MeanscriptCmd.h"
#include "MS.h"
#include "NativeTest.h"

// memory debug

#include <stdlib.h>

#ifdef MS_VS_MEM_DEBUG
#include <crtdbg.h>
#endif

using namespace std;
using namespace meanscript;

void printVersion()
{
	MSPRINT("Meanscript CLI, ").print(MS_BUILD_INFO).endLine();
}

void printCommandHelp(const char * cmd, const char * args, const char * description)
{
	MSPRINT("  ").print(cmd).print(" ").print(args).print("\n      ").print(description).endLine();
}

void printHelp()
{
	printVersion();
	MSPRINT("Command line interface\n");
	MSPRINT("USAGE:\n  mean [command] [options] <file(s)>\n");
	MSPRINT("OPTIONS:\n");
	MSPRINT("  -v, --verbose                  Print command details.\n");
	MSPRINT("COMMANDS:\n");
	
	MSPRINT("  compile <script> <output>      Compile a script and write bytecode to a file.\n");
	MSPRINT("  cr <script> <output>           Same as 'compile' but also run they bytecode.\n");
	MSPRINT("  run <bytecode>                 Read and run a bytecode.\n");
	MSPRINT("  decode <bytecode>              Read bytecode and print its content.\n");
	MSPRINT("  view <bytecode>                Run code and view values.\n");
	MSPRINT("  makejava <script> <module> <package> Generate Java classes.\n");
	MSPRINT("  test                           Run unit tests.\n");
	MSPRINT("  help                           Print help.\n");
	MSPRINT("  version                        Print version.\n");

	MSPRINT("\nEXAMPLES:\n");
	MSPRINT("Print version:                   mean version\n");
	MSPRINT("View contents of foo.mb:         mean view foo.mb\n");
	MSPRINT("Compile a file with verbose on:  mean --verbose compile bar.ms foo.mb\n");
}

enum CommandType
{
	CMD_UNKNOWN,
	
	CMD_COMPILE_AND_RUN,
	CMD_COMPILE,
	CMD_RUN,
	CMD_DECODE,
	CMD_VIEW,
	CMD_MAKE_JAVA,
	CMD_TEST,
	CMD_HELP,
	CMD_VERSION
};

int execute(int argc, char* argv[])
{  

	//int32_t ints[3];
	//ints[0] = 0x00000005;
	//ints[1] = 0x64636261;
	//ints[2] = 0x00000065;
	//std::cout << ((const char*)(ints+1)) << std::endl;

	if (argc == 1)
	{
        printHelp();
		return 1;
    }
    string source;
    string destination;
	
	CommandType commandType = CMD_UNKNOWN;

	
	int i = 1;
	string arg(argv[i++]);
	
	// read subcommand
	
	if (arg == "cr") commandType = CMD_COMPILE_AND_RUN;
	else if (arg == "compile") commandType = CMD_COMPILE;
	else if (arg == "run") commandType = CMD_RUN;
	else if (arg == "decode") commandType = CMD_DECODE;
	else if (arg == "view") commandType = CMD_VIEW;
	else if (arg == "makejava") commandType = CMD_MAKE_JAVA;
	else if (arg == "test") commandType = CMD_TEST;
	else if (arg == "help") commandType = CMD_HELP;
	else if (arg == "version") commandType = CMD_VERSION;
	else
	{
		MSPRINT("invalid subcommand: ").print(arg).endLine();
		printHelp();
		return -1;
	}
	
	// read options

    for (; i < argc; ++i)
	{
		string arg(argv[i]);

		// check if arg starts with -- or -

		if (arg.rfind("--", 0) == 0)
		{
			if (arg == "--verbose")
			{
				MSPRINT("Set verbose on.").endLine();
				setVerbose(true);
			}
			else
			{
				MSPRINT("Unknown argument: ").print(arg);
				return -1;
			}
		}
		else if (arg.rfind("-", 0) == 0)
		{
			if (arg.size() == 1) ERROR("flag arguments expected after a '-'");

			for(size_t n=1; n<arg.size(); n++)
			{
				if (arg[n] == 'v')
				{
					setVerbose(true);
				}
				else
				{
					MSPRINT("Unknown flag: ").print((arg[n]));
					return -1;
				}
			}
		}
		else break;
    }

	switch (commandType)
	{
		
	case CMD_COMPILE_AND_RUN:
	case CMD_COMPILE:
		{
			if (argc != i + 2)
			{
				MSPRINT("compile: input and output files expected");
				return -1;
			}

			MSFileInStream fis = getInput(argv[i]);

			MSCode m(fis, globalConfig.STREAM_SCRIPT);
			fis.close();
		
			MSFileOutStream fos = getOutput(argv[i+1]);
			m.writeCode(fos);
			fos.close();

			if (commandType == CMD_COMPILE_AND_RUN)
			{
				MSPRINT("run bytecode...\n\n");
				m.run();
			}
		}
		break;
	case CMD_RUN:
		{
			if (argc != i + 1) ERROR("bytecode file for input expected");
		
			MSFileInStream fis = getInput(argv[i]);
			MSCode m(fis, globalConfig.STREAM_BYTECODE);

			m.run();
		}
		break;
	case CMD_DECODE:
		{
			if (argc != i + 1) ERROR("bytecode file for input expected");
		
			MSFileInStream fis = getInput(argv[i]);
			MSCode m(fis, globalConfig.STREAM_BYTECODE);

			m.printCode();
		}
		break;
	case CMD_VIEW:
		{
			if (argc != i + 1) ERROR("bytecode file for input expected");
		
			MSFileInStream fis = getInput(argv[i]);
			MSCode m(fis, globalConfig.STREAM_BYTECODE);
			
			m.run();
			m.printData();
		}
		break;
	case CMD_MAKE_JAVA:
		{
			// makejava <script> <module> <package> <target folder> Generate Java classes.
			if (argc != i + 3)
			{
				MSPRINT("makejava: wrong number of arguments\n");
				return -1;
			}
			string script = argv[i];
			string module = argv[i + 1];
			string package = argv[i + 2];

			MSPRINT("make Java classes!")
				.print("\nscript: ").print(script)
				.print("\nmodule: ").print(module)
				.print("\npackage: ").print(package).endLine();

			MSFileInStream fis = getInput(script.c_str(), true);
			MSCode m(fis, globalConfig.STREAM_SCRIPT);
			fis.close();
			m.run();

			string outputDir = std::getenv("MS_OUTPUT");
			outputDir += meanscript::filePathSeparator();

			meanscriptcore::ClassMaker cm;
			cm.makeJava(m.getMM()->byteCode->code, package, outputDir);

		}
		break;
	case CMD_TEST:
		printVersion();
		MSPRINT("Run unit tests...\n");
		meanscriptcore::MeanscriptUnitTest::runAll();
		MSPRINT("\nTESTS DONE!\n");
		return 1;
		break;
	case CMD_HELP:
		printHelp();
		return 1;
		break;
	case CMD_VERSION:
		printVersion();
		return 1;
	default:
		MSPRINT("unknown command type: ").print(commandType).endLine();
		return -1;
	}

	return 1;

}

int main(int argc, char* argv[])
{
	
#ifdef MS_VS_MEM_DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif 

	setVerbose(false);
	
	int a = execute(argc, argv);
	
	return a;
}
