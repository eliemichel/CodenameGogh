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

QModelIndex NodeListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (isRoot(parent) && isIndexValid(row, column, parent)) {
		return createIndex(row, column, nullptr);
	}
	else {
		return QModelIndex();
	}
}

QModelIndex NodeListModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int NodeListModel::rowCount(const QModelIndex &parent) const
{
	if (isRoot(parent) && !m_graph) {
		return static_cast<int>(m_entries.size());
	}
	else {
		return 0;
	}
}

int NodeListModel::columnCount(const QModelIndex &parent) const
{
	return _ColumnCount;
}

QVariant NodeListModel::data(const QModelIndex &index, int role) const
{
	if (!isIndexValid(index)) {
		return QVariant();
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		role = columnToRole(index.column());
	}
	
	return m_entries[index.row()].data(role);
}

///////////////////////////////////////////////////////////////////////////////
// Editable QAbstractItemModel implementation

bool NodeListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!isIndexValid(index)) {
		return false;
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		role = columnToRole(index.column());
	}

	if (m_entries[index.row()].setData(role, value)) {
		dataChanged(index, index);
		return true;
	} else {
		return false;
	}
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
	if (!m_graph) return false;
	if (parent.isValid()) return false; // Only insert rows at root
	if (row < 0 || row > rowCount(parent)) return false;

	beginInsertRows(parent, row, row + count - 1);
	for (int i = 0; i < count; ++i)
	{
		m_entries.insert(m_entries.begin() + row, ModelEntry(m_graph->insertNode(row)));
	}
	endInsertRows();
	return true;
}

bool NodeListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (!m_graph) return false;
	if (parent.isValid()) return false; // Only remove rows from root
	int startRow = std::max(row, 0);
	int endRow = std::min(row + count, rowCount(parent) - 1);
	if (endRow <= startRow) return false; // Nothing to remove
	beginRemoveRows(parent, startRow, endRow);
	for (auto it = m_entries.begin() + startRow, end = m_entries.begin() + endRow + 1; it != end; ++it)
	{
		m_graph->removeNode(it->node);
	}
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

bool NodeListModel::isIndexValid(int row, int column, const QModelIndex &parent) const {
	return column >= 0 && column < columnCount(parent)
		&& row >= 0 && row < rowCount(parent);
}
bool NodeListModel::isIndexValid(const QModelIndex &index) const {
	return index.isValid() && isIndexValid(index.row(), index.column(), index.parent());
}

NodeListModel::Role NodeListModel::columnToRole(int column)
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
	m_entries.clear();
	m_entries.reserve(m_graph->edges.size());
	for (int i = 0; i < m_graph->nodes.size(); ++i)
	{
		m_entries.push_back(ModelEntry(m_graph->nodes[i]));
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