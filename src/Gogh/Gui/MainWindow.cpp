/**
* This file is part of Gogh.
* It is released under the terms of the MIT License:
*
* Copyright (c) 2018 - 2019 -- �lie Michel <elie.michel@exppad.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the �Software�), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* The Software is provided �as is�, without warranty of any kind, express or
* implied, including but not limited to the warranties of merchantability,
* fitness for a particular purpose and non-infringement. In no event shall the
* authors or copyright holders be liable for any claim, damages or other
* liability, whether in an action of contract, tort or otherwise, arising
* from, out of or in connection with the software or the use or other dealings
* in the Software.
*/

#include <QTreeView>
#include <QMenu>

#include "MainWindow.h"
#include "ParameterListView.h"
#include "ParameterListModel.h"

using namespace Gogh::Gui;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ParameterListView *view = new ParameterListView();
	ParameterListModel *model = new ParameterListModel();
	view->setModel(model);
	setCentralWidget(view);
}
