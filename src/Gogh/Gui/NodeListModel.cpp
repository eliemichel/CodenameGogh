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
#include "GraphMetaTypes.h"

using namespace Gogh::Gui;

NodeListModel::NodeListModel()
{
	// DEBUG
	auto p = std::make_shared<Node>();
	p->name = "Node_1";
	m_entries.push_back(ModelEntry(p));
	p = std::make_shared<Node>();
	p->name = "Node_2";
	m_entries.push_back(ModelEntry(p));
}

///////////////////////////////////////////////////////////////////////////////
// Basic QAbstractItemModel implementation

QModelIndex NodeListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column < 0 || column >= columnCount(parent)) return QModelIndex();
	if (row < 0 || row >= rowCount(parent)) return QModelIndex(); // out of bounds

	if (isRoot(parent))
	{
		return createIndex(row, column, nullptr);
	}
	else
	{
		return QModelIndex();
	}
}

QModelIndex NodeListModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int NodeListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have subchildren
	return static_cast<int>(m_entries.size());
}

int NodeListModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have subchildren
	return _ColumnCount;
}

QVariant NodeListModel::data(const QModelIndex &index, int role) const
{
	if (index.isValid()
		&& index.column() >= 0 && index.column() < columnCount(index.parent())
		&& index.row() >= 0 && index.row() < rowCount(index.parent()))
	{
		if (role == Qt::DisplayRole || role == Qt::EditRole)
		{
			switch (index.column())
			{
			case NameColumn:
				return QString::fromStdString(m_entries[index.row()].node->name);
			case XPosColumn:
			{
				return m_entries[index.row()].node->x;
			}
			case YPosColumn:
			{
				return m_entries[index.row()].node->y;
			}
			default:
				return QVariant();
			}
		}
		else if (role == NodePtrRole)
		{
			return QVariant::fromValue(m_entries[index.row()].node);
		}
		else if (role == ParameterModelRole)
		{
			return QVariant::fromValue(m_entries[index.row()].parameters);
		}
		else if (role == InputModelRole)
		{
			return QVariant::fromValue(m_entries[index.row()].inputs);
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
			m_entries[index.row()].node->name = value.toString().toStdString();
			dataChanged(index, index);
			return true;
		case XPosColumn:
			m_entries[index.row()].node->x = value.toFloat();
			dataChanged(index, index);
			return true;
		case YPosColumn:
			m_entries[index.row()].node->y = value.toFloat();
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
	for (int i = 0; i < count; ++i)
	{
		m_entries.insert(m_entries.begin() + row, ModelEntry(std::make_shared<Node>()));
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
	m_entries.erase(m_entries.begin() + startRow, m_entries.begin() + endRow);
	endRemoveRows();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Drag and drop

Qt::DropActions NodeListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}
