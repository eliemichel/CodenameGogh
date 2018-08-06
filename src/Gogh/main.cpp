#include "MainWindow.h"
#include "Logger.h"
#include "EnvModel.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

#include <map>
#include <string>
#include <sstream>

QString loadCss(const QString & filename);

struct Args
{
	std::string graphFilename;
	bool isGraphFilenameProvided = false;
	std::map<std::string, std::string> env;
	std::string helpMessage;
	bool displayHelpMessage = false;
};
Args parseArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	Args args = parseArgs(argc, argv);

	if (args.displayHelpMessage)
	{
		std::cout << args.helpMessage << std::endl;
		return EXIT_SUCCESS;
	}

	QApplication a(argc, argv);
	//a.setStyleSheet(loadCss(":styles/default.css"));

	QString filename = args.isGraphFilenameProvided ? QString::fromStdString(args.graphFilename) : QString();
	MainWindow w(filename);

	// Import command line env
	for (auto it = args.env.cbegin() ; it != args.env.cend() ; ++it)
	{
		w.envModel()->env()[it->first] = it->second;
	}

	w.show();

	return a.exec();
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
			args.graphFilename = value;
			args.isGraphFilenameProvided = true;
			continue;
		}

		if (param == "--help" || param == "-h")
		{
			args.displayHelpMessage = true;
			std::ostringstream ss;;
			ss
				<< "Gogh vAlpha" << std::endl
				<< "Copyright (c) 2018 -- Élie Michel & Félix David" << std::endl
				<< "" << std::endl
				<< "Usage: gogh [<filename>] [OPTIONS]" << std::endl
				<< "" << std::endl
				<< "Available options are:" << std::endl
				<< "" << std::endl
				<< "    --graph=<filename> OR -g <filename> : Open node graph from gog file <filename>" << std::endl
				<< "    --help OR -h : Display thif help message" << std::endl
				<< "    -D<varname>=<value> : Add an anvironment variable <varname> with value <value> to Gogh's environment variables register" << std::endl
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
