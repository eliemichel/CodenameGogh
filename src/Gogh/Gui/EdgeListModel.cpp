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

#include "EdgeListModel.h"
#include "Logger.h"
#include "GraphMetaTypes.h"

using namespace Gogh::Gui;

void EdgeListModel::setGraph(Gogh::GraphPtr graph)
{
	beginResetModel();
	m_graph = graph;
	reloadFromGraph();
	endResetModel();
}

///////////////////////////////////////////////////////////////////////////////
// Basic QAbstractTableModel implementation

int EdgeListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have subchildren
	return static_cast<int>(m_entries.size());
}

int EdgeListModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have subchildren
	return _ColumnCount;
}

QVariant EdgeListModel::data(const QModelIndex &index, int role) const
{
	if (index.isValid()
		&& index.column() >= 0 && index.column() < columnCount(index.parent())
		&& index.row() >= 0 && index.row() < rowCount(index.parent()))
	{
		if (role == Qt::DisplayRole || role == Qt::EditRole)
		{
			switch (index.column())
			{
			case OriginNodeColumn:
				return m_entries[index.row()].originNode;
			case OriginOutputColumn:
				return m_entries[index.row()].originOutput;
			case DestinationNodeColumn:
				return m_entries[index.row()].destinationNode;
			case DestinationInputColumn:
				return m_entries[index.row()].destinationInput;
			default:
				return QVariant();
			}
		}
	}
	return QVariant();
}

///////////////////////////////////////////////////////////////////////////////
// Editable QAbstractTableModel implementation

bool EdgeListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole && index.isValid()
		&& index.column() >= 0 && index.column() < columnCount(index.parent())
		&& index.row() >= 0 && index.row() < rowCount(index.parent()))
	{
		switch (index.column())
		{
		// TODO
		default:
			return false;
		}
	}
	return false;
}

Qt::ItemFlags EdgeListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags itemFlags = QAbstractTableModel::flags(index);

	if (index.isValid())
	{
		itemFlags |= Qt::ItemIsEditable;
		itemFlags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	}

	return itemFlags;
}

///////////////////////////////////////////////////////////////////////////////
// Headers QAbstractTableModel implementation

QVariant EdgeListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) return QVariant();

	switch (section)
	{
	case OriginNodeColumn:
		return "Origin Node";
	case OriginOutputColumn:
		return "Origin Output";
	case DestinationNodeColumn:
		return "Destination Node";
	case DestinationInputColumn:
		return "Destination Input";
	default:
		return QVariant();
	}
}

///////////////////////////////////////////////////////////////////////////////
// Resizable QAbstractTableModel implementation

bool EdgeListModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid()) return false; // Only insert rows at root
	if (row < 0 || row > rowCount(parent)) return false;

	beginInsertRows(parent, row, row + count - 1);
	for (int i = 0; i < count; ++i)
	{
		ModelEntry entry;
		entry.edge = m_graph->addEdge(nullptr, nullptr);
		m_entries.insert(m_entries.begin() + row, entry);
	}
	endInsertRows();
	return true;
}

bool EdgeListModel::removeRows(int row, int count, const QModelIndex &parent)
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

Qt::DropActions EdgeListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

///////////////////////////////////////////////////////////////////////////////
// Internal sync'ing with graph backend

// Damn, this is painful and error-prone, I wonder if there is really a need for
// a pointer-based backend...
void EdgeListModel::reloadFromGraph() noexcept
{
	// 1. Fill node map for quicker access in second step
	std::map<NodePtr,int> nodeMap;
	for (int i = 0 ; i < m_graph->nodes.size() ; ++i)
	{
		nodeMap[m_graph->nodes[i]] = i;
	}

	// Iterate over edges to get their data as indices
	m_entries.resize(m_graph->edges.size());
	for (int i = 0 ; i < m_graph->edges.size() ; ++i)
	{
		ModelEntry entry;
		entry.edge = m_graph->edges[i];
		if (NodeOutputPtr originSlot = m_graph->edges[i]->getOrigin())
		{
			// Look for output index
			if (NodePtr originNode = originSlot->getNode())
			{
				for (int j = 0; j < originNode->outputs.size(); ++j)
				{
					if (originNode->outputs[j] == originSlot)
					{
						entry.originNode = nodeMap[originNode];
						entry.originOutput = j;
					}
				}
			}
		}
		if (NodeInputPtr destinationSlot = m_graph->edges[i]->getDestination())
		{
			// Look for input index
			if (NodePtr destinationNode = destinationSlot->getNode())
			{
				for (int j = 0; j < destinationNode->inputs.size(); ++j)
				{
					if (destinationNode->inputs[j] == destinationSlot)
					{
						entry.destinationNode = nodeMap[destinationNode];
						entry.destinationInput = j;
					}
				}
			}
		}
		m_entries[i] = entry;
	}
}
