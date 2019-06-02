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

#include <QTreeView>
#include <QMenu>
#include <QSplitter>

#include "MainWindow.h"
#include "ParameterListView.h"
#include "ParameterListModel.h"
#include "NodeListModel.h"
#include "EdgeListModel.h"
#include "Graph.h"
#include "GraphMetaTypes.h"

using namespace Gogh::Gui;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
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

	// Models
	EdgeListModel *edgeModel = new EdgeListModel();
	edgeModel->setGraph(graph);

	NodeListModel *nodeModel = new NodeListModel();
	nodeModel->setGraph(graph);

	// Views
	ParameterListView *edgeView = new ParameterListView();
	edgeView->setModel(edgeModel);

	ParameterListView *nodeView = new ParameterListView();
	nodeView->setModel(nodeModel);

	paramView = new ParameterListView();
	inputView = new ParameterListView();
	outputView = new ParameterListView();

	connect(nodeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(onCurrentNodeChanged(const QModelIndex &, const QModelIndex &)));

	// Layout
	QSplitter *splitter = new QSplitter();
	splitter->addWidget(edgeView);
	splitter->addWidget(nodeView);
	QSplitter *vsplitter = new QSplitter();
	vsplitter->setOrientation(Qt::Vertical);
	vsplitter->addWidget(paramView);
	vsplitter->addWidget(inputView);
	vsplitter->addWidget(outputView);
	splitter->addWidget(vsplitter);

	setCentralWidget(splitter);
}

void MainWindow::onCurrentNodeChanged(const QModelIndex & current, const QModelIndex & previous)
{
	auto pParamModel = current.data(NodeListModel::ParameterModelRole).value<std::shared_ptr<ParameterListModel>>();
	paramView->setModel(pParamModel.get());
	auto pInputModel = current.data(NodeListModel::InputModelRole).value<std::shared_ptr<NodeInputListModel>>();
	inputView->setModel(pInputModel.get());
	auto pOutputModel = current.data(NodeListModel::OutputModelRole).value<std::shared_ptr<NodeOutputListModel>>();
	outputView->setModel(pOutputModel.get());
}
