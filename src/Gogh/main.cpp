/**
* This file is part of Gogh.
* It is released under the terms of the MIT License:
*
* Copyright (c) 2018 - 2019 -- Élie Michel <elie.michel@exppad.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* The Software is provided “as is”, without warranty of any kind, express or
* implied, including but not limited to the warranties of merchantability,
* fitness for a particular purpose and non-infringement. In no event shall the
* authors or copyright holders be liable for any claim, damages or other
* liability, whether in an action of contract, tort or otherwise, arising
* from, out of or in connection with the software or the use or other dealings
* in the Software.
*/

#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QApplication>
#include <QQmlEngine>
#include <QQmlContext>

#include "Logger.h"
#include "Parameter.h"
#include "Graph.h"
#include "Gui/MainWindow.h"
#include "Gui/NodeListModel.h"

using namespace Gogh;

int main(int argc, char *argv[])
{
	// DEBUG Mock graph
	GraphPtr graph = std::make_shared<Graph>();

	NodePtr n1 = graph->addNode();
	n1->name = "Node_1";
	NodeOutputPtr o1 = n1->addOutput();
	o1->name = "Video Out";
	NodeOutputPtr o2 = n1->addOutput();
	o2->name = "Audio Out";

	NodePtr n2 = graph->addNode();
	n2->name = "Node_2";
	NodeInputPtr i1 = n2->addInput();
	i1->name = "Video In";
	NodeInputPtr i2 = n2->addInput();
	i2->name = "Audio In";

	auto p = std::make_shared<Parameter>();
	p->setName("Param_1");
	n1->parameters.push_back(p);
	p = std::make_shared<Parameter>();
	p->setName("Param_2");
	n1->parameters.push_back(p);

	graph->addEdge(o1, i1);
	graph->addEdge(o2, i2);

	//*
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;

	qmlRegisterType<Gogh::Gui::NodeListModel>("Gogh", 1, 0, "NodeListModel");
	qmlRegisterType<Gogh::Gui::ParameterListModel>("Gogh", 1, 0, "ParameterListModel");
	Gui::NodeListModel *nodeModel = new Gui::NodeListModel();
	nodeModel->setGraph(graph);
	engine.rootContext()->setContextProperty("nodeListModel", nodeModel);

	engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
	if (engine.rootObjects().isEmpty())
	{
		return -1;
	}
	return app.exec();
	/*/
	QApplication app(argc, argv);

	Gogh::Gui::MainWindow window;
	window.resize(800, 600);
	window.show();

	return app.exec();
	//*/
}
