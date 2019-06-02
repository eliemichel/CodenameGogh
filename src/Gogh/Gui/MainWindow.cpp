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
#include "Graph.h"
#include "GraphMetaTypes.h"

using namespace Gogh::Gui;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	NodeListModel *nodeModel = new NodeListModel();
	ParameterListView *nodeView = new ParameterListView();
	nodeView->setModel(nodeModel);

	/*
	NodePtr node = std::make_shared<Node>();
	auto p = std::make_shared<Parameter>();
	p->setName("Param_1");
	node->parameters.push_back(p);
	p = std::make_shared<Parameter>();
	p->setName("Param_2");
	node->parameters.push_back(p);
	*/

	paramModel = new ParameterListModel();
	NodePtr node = nodeModel->index(0, 0).data(NodeListModel::NodePtrRole).value<Gogh::NodePtr>();
	auto p = std::make_shared<Parameter>();
	p->setName("Param_1");
	node->parameters.push_back(p);
	p = std::make_shared<Parameter>();
	p->setName("Param_2");
	node->parameters.push_back(p);

	paramView = new ParameterListView();
	paramView->setModel(paramModel);

	connect(nodeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(onCurrentNodeChanged(const QModelIndex &, const QModelIndex &)));

	QSplitter *splitter = new QSplitter();
	splitter->addWidget(nodeView);
	splitter->addWidget(paramView);

	setCentralWidget(splitter);
}

void MainWindow::onCurrentNodeChanged(const QModelIndex & current, const QModelIndex & previous)
{
	// Option A
	//Gogh::NodePtr node = current.data(NodeListModel::NodePtrRole).value<Gogh::NodePtr>();
	//paramModel->setNode(node);

	// Option B
	auto m = current.data(NodeListModel::ParameterModelRole).value<std::shared_ptr<ParameterListModel>>();
	paramView->setModel(m.get());
}
