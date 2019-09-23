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

void NodeListModel::setGraph(Gogh::GraphPtr graph)
{
	beginResetModel();
	m_graph = graph;
	reloadFromGraph();
	endResetModel();
}

///////////////////////////////////////////////////////////////////////////////
// Basic QAbstractItemModel implementation

int NodeListModel::columnCount(const QModelIndex &parent) const
{
	return _ColumnCount;
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
// QML Roles

QHash<int, QByteArray> NodeListModel::roleNames() const
{
	QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
	names[NameRole] = "name";
	names[XPosRole] = "x";
	names[YPosRole] = "y";
	names[RawParameterModelRole] = "parameters";
	names[RawInputModelRole] = "inputs";
	names[RawOutputModelRole] = "outputs";
	return names;
}

///////////////////////////////////////////////////////////////////////////////
// Utils

NodeListModel::AbstractModelEntry * NodeListModel::createEntry(int row)
{
	if (!m_graph) return nullptr;
	return new ModelEntry(m_graph->insertNode(row));
}

bool NodeListModel::destroyEntry(AbstractModelEntry *entry)
{
	if (!m_graph) return false;
	m_graph->removeNode(static_cast<ModelEntry*>(entry)->node);
	return true;
}

int NodeListModel::columnToRole(int column) const
{
	switch (column)
	{
	case NameColumn:
		return NameRole;
	case XPosColumn:
		return XPosRole;
	case YPosColumn:
		return YPosRole;
	default:
		return InvalidRole;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Internal sync'ing with graph backend

void NodeListModel::reloadFromGraph() noexcept
{
	// Detach from previous graph without deleting the underlying data
	// which is why we don't call removeRows nor destroyEntry
	for (int i = 0; i < m_entries.size(); ++i)
	{
		delete m_entries[i];
	}
	m_entries.clear();

	// Attach to existing data without creating it, which is why we don't
	// call insertRows nor createEntry
	m_entries.reserve(m_graph->nodes.size());
	for (int i = 0; i < m_graph->nodes.size(); ++i)
	{
		m_entries.push_back(new ModelEntry(m_graph->nodes[i]));
	}
}

///////////////////////////////////////////////////////////////////////////////
// ModelEntry methods

QVariant NodeListModel::ModelEntry::data(int role) const {
	switch (role)
	{
	case NameRole:
		return QString::fromStdString(node->name);

	case XPosRole:
		return node->x;

	case YPosRole:
		return node->y;

	case NodePtrRole:
		return QVariant::fromValue(node);

	case ParameterModelRole:
		return QVariant::fromValue(parametersModel);

	case InputModelRole:
		return QVariant::fromValue(inputsModel);

	case OutputModelRole:
		return QVariant::fromValue(outputsModel);
		
	case RawParameterModelRole:
		return QVariant::fromValue(parametersModel.get());

	case RawInputModelRole:
		return QVariant::fromValue(inputsModel.get());

	case RawOutputModelRole:
		return QVariant::fromValue(outputsModel.get());

	default:
		return QVariant();
	}
}

bool NodeListModel::ModelEntry::setData(int role, QVariant value) {
	switch (role)
	{
	case NameRole:
		node->name = value.toString().toStdString();
		return true;

	case XPosRole:
		node->x = value.toFloat();
		return true;

	case YPosRole:
		node->y = value.toFloat();
		return true;

	default:
		return false;
	}
}