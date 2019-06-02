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

#include "NodeListModel.h"
#include "Logger.h"

using namespace Gogh::Gui;

NodeListModel::NodeListModel()
{
	// DEBUG
	auto p = std::make_shared<Node>();
	p->name = "Node_1";
	m_nodes.push_back(p);
	p = std::make_shared<Node>();
	p->name = "Node_2";
	m_nodes.push_back(p);
}

///////////////////////////////////////////////////////////////////////////////
// Basic QAbstractItemModel implementation

QModelIndex NodeListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column < 0 || column >= columnCount(parent)) return QModelIndex();
	if (parent.isValid()) return QModelIndex(); // Children don't have sub-children
	if (row < 0 || row >= rowCount(parent)) return QModelIndex(); // out of bounds
	return createIndex(row, column, nullptr);
}

QModelIndex NodeListModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int NodeListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have subchildren
	return static_cast<int>(m_nodes.size());
}

int NodeListModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have subchildren
	return _ColumnCount;
}

QVariant NodeListModel::data(const QModelIndex &index, int role) const
{
	if ((role == Qt::DisplayRole || role == Qt::EditRole) && index.isValid()
		&& index.column() >= 0 && index.column() < columnCount(index.parent())
		&& index.row() >= 0 && index.row() < rowCount(index.parent()))
	{
		switch (index.column())
		{
		case NameColumn:
			return QString::fromStdString(m_nodes[index.row()]->name);
		case XPosColumn:
		{
			return m_nodes[index.row()]->x;
		}
		case YPosColumn:
		{
			return m_nodes[index.row()]->y;
		}
		default:
			return QVariant();
		}
	}
	return QVariant();
}

///////////////////////////////////////////////////////////////////////////////
// Editable QAbstractItemModel implementation

bool NodeListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole && index.isValid()
		&& index.column() >= 0 && index.column() < columnCount(index.parent())
		&& index.row() >= 0 && index.row() < rowCount(index.parent()))
	{
		switch (index.column())
		{
		case NameColumn:
			m_nodes[index.row()]->name = value.toString().toStdString();
			dataChanged(index, index);
			return true;
		case XPosColumn:
			m_nodes[index.row()]->x = value.toFloat();
			dataChanged(index, index);
			return true;
		case YPosColumn:
			m_nodes[index.row()]->y = value.toFloat();
			dataChanged(index, index);
			return true;
		default:
			return false;
		}
	}
	return false;
}

Qt::ItemFlags NodeListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags itemFlags = QAbstractItemModel::flags(index);

	if (index.isValid())
	{
		itemFlags |= Qt::ItemIsEditable;
		itemFlags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	}

	return itemFlags;
}

///////////////////////////////////////////////////////////////////////////////
// Headers QAbstractItemModel implementation

QVariant NodeListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) return QVariant();

	switch (section)
	{
	case NameColumn:
		return "Name";
	case XPosColumn:
		return "X";
	case YPosColumn:
		return "Y";
	default:
		return QVariant();
	}
}

///////////////////////////////////////////////////////////////////////////////
// Resizable QAbstractItemModel implementation

bool NodeListModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid()) return false; // Only insert rows at root
	if (row < 0 || row > rowCount(parent)) return false;

	beginInsertRows(parent, row, row + count - 1);
	m_nodes.insert(m_nodes.begin() + row, count, nullptr);
	for (int i = row; i < row + count; ++i)
	{
		m_nodes[i] = std::make_shared<Node>();
	}
	endInsertRows();
	return true;
}

bool NodeListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid()) return false; // Only remove rows from root
	int startRow = std::max(row, 0);
	int endRow = std::min(row + count, rowCount(parent) - 1);
	if (endRow <= startRow) return false; // Nothing to remove
	beginRemoveRows(parent, startRow, endRow);
	m_nodes.erase(m_nodes.begin() + startRow, m_nodes.begin() + endRow);
	endRemoveRows();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Drag and drop

Qt::DropActions NodeListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}
