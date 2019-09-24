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

#include "NodeInputListModel.h"
#include "Logger.h"
#include "GraphMetaTypes.h"

using namespace Gogh::Gui;


void NodeInputListModel::setNode(Gogh::NodePtr node)
{
	beginResetModel();
	m_node = node;
	reloadFromNode();
	endResetModel();
}

///////////////////////////////////////////////////////////////////////////////
// Basic QAbstractTableModel implementation

int NodeInputListModel::columnCount(const QModelIndex &parent) const
{
	return _ColumnCount;
}

///////////////////////////////////////////////////////////////////////////////
// Headers QAbstractItemModel implementation

QVariant NodeInputListModel::headerData(int section, Qt::Orientation orientation, int role) const
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
// Implementation of AbstractListModel

NodeInputListModel::AbstractModelEntry * NodeInputListModel::createEntry(int row)
{
	m_node->inputs.insert(m_node->inputs.begin() + row, 1, nullptr);
	m_node->inputs[row] = std::make_shared<NodeInput>();
	return new NodeInputModel(m_node->inputs[row]);
}

bool NodeInputListModel::destroyEntry(int row, AbstractModelEntry * entry)
{
	if (row >= m_node->parameters.size()) return false;
	NodeInputModel* inputModel = static_cast<NodeInputModel*>(entry);
	m_node->inputs.erase(m_node->inputs.begin() + row);
	delete inputModel;
	return true;
}

int NodeInputListModel::columnToRole(int column) const
{
	switch (column)
	{
	case NameColumn:
		return NameRole;
	case TypeColumn:
		return TypeRole;
	case ViewXColumn:
		return ViewXRole;
	case ViewYColumn:
		return ViewYRole;
	default:
		return InvalidRole;
	}
}

void NodeInputListModel::reloadFromNode() noexcept
{
	// Detach from previous graph without deleting the underlying data
	// which is why we don't call removeRows nor destroyEntry
	for (int i = 0; i < m_entries.size(); ++i)
	{
		delete m_entries[i];
	}
	m_entries.clear();

	if (!m_node) return;

	// Attach to existing data without creating it, which is why we don't
	// call insertRows nor createEntry
	m_entries.reserve(m_node->inputs.size());
	for (int i = 0; i < m_node->inputs.size(); ++i)
	{
		m_entries.push_back(new NodeInputModel(m_node->inputs[i]));
	}
}

///////////////////////////////////////////////////////////////////////////////
// NodeInputModel methods

QVariant NodeInputListModel::NodeInputModel::data(int role) const {
	switch (role)
	{
	case NameColumn:
		return QString::fromStdString(m_input->name);
	case TypeColumn:
		return QVariant(); // TODO
	case ViewXColumn:
		return m_x;
	case ViewYColumn:
		return m_y;
	default:
		return QVariant();
	}
}

bool NodeInputListModel::NodeInputModel::setData(int role, QVariant value) {
	switch (role)
	{
	case NameColumn:
		m_input->name = value.toString().toStdString();
		return true;
	case TypeColumn:
		// TODO
		return false;
	case ViewXColumn:
		m_x = value.toFloat();
		return true;
	case ViewYColumn:
		m_y = value.toFloat();
		return true;
	default:
		return false;
	}
}
