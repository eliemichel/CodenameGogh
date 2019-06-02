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

#include <QMenu>

#include "ParameterListView.h"
#include "ParameterListModel.h"
#include "ParameterTypeDelegate.h"

using namespace Gogh::Gui;

ParameterListView::ParameterListView(QWidget *parent)
	: QTreeView(parent)
	, m_contextMenu(new QMenu())
{
	ParameterTypeDelegate *typeDelegate = new ParameterTypeDelegate();

	setContextMenuPolicy(Qt::CustomContextMenu);
	setItemDelegateForColumn(ParameterListModel::TypeColumn, typeDelegate);
	setUniformRowHeights(true); // optimization
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);

	// Context menu
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onContextMenu(const QPoint &)));
	m_contextMenu->addAction("Remove", this, SLOT(removeParameter())); // DEBUG
	m_contextMenu->addAction("Insert above", this, SLOT(insertParameterAbove())); // DEBUG
	m_contextMenu->addAction("Insert bellow", this, SLOT(insertParameterBellow())); // DEBUG
}

///////////////////////////////////////////////////////////////////////////////
// Slots

void ParameterListView::onContextMenu(const QPoint & point) // DEBUG
{
	QModelIndex index = indexAt(point);
	m_contextMenuParameterIndex = index;
	m_contextMenu->actions().at(0)->setEnabled(index.isValid());
	m_contextMenu->actions().at(1)->setEnabled(index.isValid());
	m_contextMenu->exec(viewport()->mapToGlobal(point));
}

void ParameterListView::removeParameter() // DEBUG
{
	if (m_contextMenuParameterIndex.isValid())
	{
		model()->removeRow(m_contextMenuParameterIndex.row(), m_contextMenuParameterIndex.parent());
		m_contextMenuParameterIndex = QModelIndex();
	}
}

void ParameterListView::insertParameterAbove() // DEBUG
{
	if (m_contextMenuParameterIndex.isValid())
	{
		model()->insertRow(m_contextMenuParameterIndex.row(), m_contextMenuParameterIndex.parent());
	}
}

void ParameterListView::insertParameterBellow() // DEBUG
{
	if (m_contextMenuParameterIndex.isValid())
	{
		model()->insertRow(m_contextMenuParameterIndex.row() + 1, m_contextMenuParameterIndex.parent());
	}
	else
	{
		model()->insertRow(model()->rowCount(), m_contextMenuParameterIndex.parent());
	}
}
