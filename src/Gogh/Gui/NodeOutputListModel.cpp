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

#include "NodeOutputListModel.h"
#include "Logger.h"
#include "GraphMetaTypes.h"

using namespace Gogh::Gui;


void NodeOutputListModel::setNode(Gogh::NodePtr node)
{
	beginResetModel();
	m_node = node;
	m_viewData.clear();
	if (node) {
		m_viewData.resize(node->outputs.size());
	}
	endResetModel();
}

///////////////////////////////////////////////////////////////////////////////
// Basic QAbstractTableModel implementation

int NodeOutputListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have sub-children
	return m_node ? static_cast<int>(m_node->outputs.size()) : 0;
}

int NodeOutputListModel::columnCount(const QModelIndex &parent) const
{
	return _ColumnCount;
}

QVariant NodeOutputListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		switch (index.column())
		{
		case NameColumn:
			return QString::fromStdString(m_node->outputs[index.row()]->name);
		case TypeColumn:
			return QVariant(); // TODO
		case ViewXColumn:
			return m_viewData[index.row()].x;
		case ViewYColumn:
			return m_viewData[index.row()].y;
		default:
			return QVariant();
		}
	}

	return QVariant();
}

///////////////////////////////////////////////////////////////////////////////
// Editable QAbstractTableModel implementation

bool NodeOutputListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid()) return false;
	if (role == Qt::EditRole)
	{
		switch (index.column())
		{
		case NameColumn:
			m_node->outputs[index.row()]->name = value.toString().toStdString();
			dataChanged(index, index);
			return true;
		case TypeColumn:
			// TODO
			return false;
		case ViewXColumn:
			m_viewData[index.row()].x = value.toFloat();
			return true;
		case ViewYColumn:
			m_viewData[index.row()].y = value.toFloat();
			return true;
		default:
			return false;
		}
	}
	return false;
}

Qt::ItemFlags NodeOutputListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags itemFlags = QAbstractItemModel::flags(index);

	if (index.isValid())
	{
		itemFlags |= Qt::ItemIsEditable;
		//itemFlags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	}

	return itemFlags;
}

///////////////////////////////////////////////////////////////////////////////
// Headers QAbstractItemModel implementation

QVariant NodeOutputListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) return QVariant();

	switch (section)
	{
	case NameColumn:
		return "Name";
	case TypeColumn:
		return "Type";
	case ViewXColumn:
		return "View X";
	case ViewYColumn:
		return "View Y";
	default:
		return QVariant();
	}
}

///////////////////////////////////////////////////////////////////////////////
// Resizable QAbstractTableModel implementation

bool NodeOutputListModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid()) return false; // Only insert rows at root
	if (row < 0 || row > rowCount(parent)) return false;

	beginInsertRows(parent, row, row + count - 1);
	std::vector<NodeOutputPtr> & l = m_node->outputs;
	l.insert(l.begin() + row, count, nullptr);
	m_viewData.insert(m_viewData.begin() + row, count, ViewDataEntry());
	for (int i = row; i < row + count; ++i)
	{
		l[i] = std::make_shared<Gogh::NodeOutput>();
	}
	endInsertRows();
	return true;
}

bool NodeOutputListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid()) return false; // Only remove rows from root
	int startRow = std::max(row, 0);
	int endRow = std::min(row + count, rowCount(parent) - 1);
	if (endRow <= startRow) return false; // Nothing to remove
	beginRemoveRows(parent, startRow, endRow);
	std::vector<NodeOutputPtr> & l = m_node->outputs;
	l.erase(l.begin() + startRow, l.begin() + endRow);
	m_viewData.erase(m_viewData.begin() + startRow, m_viewData.begin() + endRow);
	endRemoveRows();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Drag and drop

Qt::DropActions NodeOutputListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}
