#include "ArgParse.h"
#include "mainGui.h"

#include <iostream>

int oldMainGui(const ArgParse & args);
int mainCmd(const ArgParse & args);

int main(int argc, char *argv[])
{
	ArgParse args = ArgParse::parse(argc, argv);

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

#include "Logger.h"
#include "EnvModel.h"
#include "NodeGraphModel.h"
#include "Node.h"
#include "Nodes/OutputNode.h"
#include "RenderProcess.h"

int mainCmd(const ArgParse & args)
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
	OutputNode *outputNode = node->type == NodeType::NODE_OUTPUT ? static_cast<OutputNode*>(node) : nullptr;
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

#include "MainWindow.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

static QString loadCss(const QString & filename)
{
	QFile file(filename);
	file.open(QIODevice::ReadOnly);
	QTextStream in(&file);
	QString s = in.readAll();
	file.close();
	return s;
}

int oldMainGui(const ArgParse & args)
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
