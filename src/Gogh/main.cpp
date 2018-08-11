#include "MainWindow.h"
#include "Logger.h"
#include "EnvModel.h"
#include "NodeGraphModel.h"
#include "Node.h"
#include "Nodes/OutputNode.h"
#include "RenderProcess.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

#include <map>
#include <string>
#include <sstream>

QString loadCss(const QString & filename);

struct Args
{
	int argc;
	char **argv;
	std::string graphFilename;
	bool isGraphFilenameProvided = false;
	std::map<std::string, std::string> env;
	std::string helpMessage;
	bool displayHelpMessage = false;
	std::string renderNodename;
	bool runNonInteractive = false;
};
Args parseArgs(int argc, char *argv[]);

int mainGui(const Args & args);
int mainCmd(const Args & args);

int main(int argc, char *argv[])
{
	Args args = parseArgs(argc, argv);

	if (args.displayHelpMessage)
	{
		std::cout << args.helpMessage << std::endl;
		return EXIT_SUCCESS;
	}

	if (args.runNonInteractive)
	{
		return mainCmd(args);
	}
	else
	{
		return mainGui(args);
	}
}

int mainGui(const Args & args)
{
	// Import command line env
	EnvModel envModel;
	for (auto it = args.env.cbegin(); it != args.env.cend(); ++it)
	{
		envModel.at(it->first) = it->second;
	}

	QString filename = args.isGraphFilenameProvided ? QString::fromStdString(args.graphFilename) : QString();

	int argc = args.argc;
	QApplication a(argc, args.argv);
	//a.setStyleSheet(loadCss(":styles/default.css"));

	MainWindow w(&envModel, filename);

	w.show();

	return a.exec();
}

int mainCmd(const Args & args)
{
	// Import command line env
	EnvModel envModel;
	for (auto it = args.env.cbegin(); it != args.env.cend(); ++it)
	{
		envModel.at(it->first) = it->second;
	}

	QString filename = args.isGraphFilenameProvided ? QString::fromStdString(args.graphFilename) : QString();

	NodeGraphModel model;
	model.setEnvModel(&envModel);

	if (filename.isNull())
	{
		ERR_LOG << "You must provide a graph file when running non interactive";
		return EXIT_FAILURE;
	}

	if (!model.LoadGraph(filename))
	{
		return EXIT_FAILURE;
	}

	const QModelIndex & outputIndex = model.findByName(args.renderNodename);
	if (!outputIndex.isValid())
	{
		ERR_LOG << "Output node not found: " << args.renderNodename;
		return EXIT_FAILURE;
	}

	Node *node = model.node(outputIndex.row());
	// TODO: move this cast to a method in model
	OutputNode *outputNode = node->type == NodeGraphModel::NODE_OUTPUT ? static_cast<OutputNode*>(node) : nullptr;
	if (!outputNode)
	{
		ERR_LOG << "Node '" << args.renderNodename << "' is not an output node";
		return EXIT_FAILURE;
	}

	RenderCommand cmd;
	if (!outputNode->buildRenderCommand(-1, cmd))
	{
		ERR_LOG << "Could not build render command";
		return EXIT_FAILURE;
	}

	RenderProcess job(cmd.cmd);
	job.start();
	if (!job.waitForFinished())
	{
		ERR_LOG << "Render job failed";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

QString loadCss(const QString & filename)
{
	QFile file(filename);
	file.open(QIODevice::ReadOnly);
	QTextStream in(&file);
	QString s = in.readAll();
	file.close();
	return s;
}

Args parseArgs(int argc, char *argv[])
{
	Args args;
	args.argc = argc;
	args.argv = argv;
	for (int i = 1 ; i < argc ; ++i)
	{
		std::string value;
		std::string param = argv[i];
		bool wasValueFound = false;
		if (param.size() >= 2 && param[0] == '-' && param[1] == '-')
		{
			size_t j = param.find('=');
			if (j != std::string::npos)
			{
				value = param.substr(j + 1);
				param = param.substr(0, j);
				wasValueFound = true;
			}
		}
		else if (param.size() >= 1 && param[0] == '-')
		{
			if (param.size() > 2)
			{
				value = param.substr(2);
				param = param.substr(0, 2);
				wasValueFound = true;
			}
		}
		else
		{
			// unparsed arguments are scene files to open
			args.graphFilename = argv[i];
			args.isGraphFilenameProvided = true;
			continue;
		}

		if (param == "--help" || param == "-h")
		{
			args.displayHelpMessage = true;
			std::ostringstream ss;;
			ss
				<< "Gogh vAlpha" << std::endl
				<< "Copyright (c) 2018 -- �lie Michel & F�lix David" << std::endl
				<< "" << std::endl
				<< "Usage: gogh [<filename>] [OPTIONS]" << std::endl
				<< "" << std::endl
				<< "Available options are:" << std::endl
				<< "" << std::endl
				<< "    --graph=<filename> OR -g <filename> : Open node graph from gog file <filename>" << std::endl
				<< "    --help OR -h : Display thif help message" << std::endl
				<< "    -D<varname>=<value> : Add an anvironment variable <varname> with value <value> to Gogh's environment variables register" << std::endl
				<< "    --render=<nodename> OR -r <nodename> : Run in non interactive mode and directly renders the node called <nodename>" << std::endl
				<< "" << std::endl
				<< "Report bugs to: https://github.com/eliemichel/CodenameGogh" << std::endl
				;
			args.helpMessage = ss.str();
		}

		if (!wasValueFound)
		{
			++i;
			if (i >= argc)
			{
				WARN_LOG << "Ignored parameter (unexpected end of command line): " << param;
				continue;
			}
			else
			{
				value = argv[i];
			}
		}

		if (param == "--graph" || param == "-g")
		{
			args.graphFilename = value;
			args.isGraphFilenameProvided = true;
		}
		else if (param == "--render" || param == "-r")
		{
			args.renderNodename = value;
			args.runNonInteractive = true;
		}
		else if (param == "-D")
		{
			std::string key;
			size_t j = value.find('=');
			if (j != std::string::npos)
			{
				key = value.substr(0, j);
				value = value.substr(j + 1);
			}
			else
			{
				++i;
				if (i >= argc)
				{
					WARN_LOG << "Ignored parameter (unexpected end of command line): -D" << value;
					continue;
				}
				else
				{
					key = value;
					value = argv[i];
				}
			}
			args.env[key] = value;
		}
	}

	return args;
}
