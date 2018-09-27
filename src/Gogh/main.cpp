#include "ArgParse.h"
#include "MainEventLoop.h"
#include "mainGui.h"

#include <iostream>

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
#include "Graph.h"
#include "Node.h"
#include "NodeType.h"
#include "Nodes/OutputNode.h"
#include "RenderProcess.h"

#include <string>

int mainCmd(const ArgParse & args)
{
	// Import command line env
	/*
	EnvModel envModel;
	for (auto it = args.env.cbegin(); it != args.env.cend(); ++it)
	{
		envModel.at(it->first) = it->second;
	}
	*/

	std::string filename = args.isGraphFilenameProvided ? args.graphFilename : "";

	Graph graph;

	if (filename.empty())
	{
		ERR_LOG << "You must provide a graph file when running non interactive";
		return EXIT_FAILURE;
	}

	if (!graph.load(filename))
	{
		return EXIT_FAILURE;
	}

	Node * node = graph.findNodeByName(args.renderNodename);
	if (!node)
	{
		ERR_LOG << "Output node not found: " << args.renderNodename;
		return EXIT_FAILURE;
	}

	// TODO: move this cast to a method in model
	OutputNode *outputNode = node->type() == NodeType::NODE_OUTPUT ? static_cast<OutputNode*>(node) : nullptr;
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
