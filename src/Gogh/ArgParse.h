#ifndef H_ARGPARSE
#define H_ARGPARSE

#include <string>
#include <map>

class ArgParse
{
public:
	int argc;
	char **argv;
	std::string graphFilename;
	bool isGraphFilenameProvided = false;
	std::map<std::string, std::string> env;
	std::string helpMessage;
	bool displayHelpMessage = false;
	std::string renderNodename;
	bool runNonInteractive = false;

public:
	static ArgParse parse(int argc, char *argv[]);
};

#endif // H_ARGPARSE
