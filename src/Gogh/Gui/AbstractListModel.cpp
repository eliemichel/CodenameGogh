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

#include "AbstractListModel.h"

using namespace Gogh::Gui;

///////////////////////////////////////////////////////////////////////////////
// Basic QAbstractItemModel implementation

QModelIndex AbstractListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (isRoot(parent) && isIndexValid(row, column, parent)) {
		return createIndex(row, column, nullptr);
	}
	else {
		return QModelIndex();
	}
}

QModelIndex AbstractListModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int AbstractListModel::rowCount(const QModelIndex &parent) const
{
	if (isRoot(parent)) {
		return static_cast<int>(m_entries.size());
	}
	else {
		return 0;
	}
}

QVariant AbstractListModel::data(const QModelIndex &index, int role) const
{
	if (!isIndexValid(index)) {
		return QVariant();
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		role = columnToRole(index.column());
	}
	
	return m_entries[index.row()]->data(role);
}

///////////////////////////////////////////////////////////////////////////////
// Editable QAbstractItemModel implementation

bool AbstractListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!isIndexValid(index)) {
		return false;
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		role = columnToRole(index.column());
	}

	if (m_entries[index.row()]->setData(role, value)) {
		dataChanged(index, index);
		return true;
	} else {
		return false;
	}
}

Qt::ItemFlags AbstractListModel::flags(const QModelIndex &index) const
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
// Resizable QAbstractItemModel implementation

bool AbstractListModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (!isRoot(parent)) return false; // Only insert rows at root
	if (row < 0 || row > rowCount(parent)) return false;

	std::vector<AbstractModelEntry*> newEntries;
	newEntries.reserve(count);
	for (int i = 0; i < count; ++i)
	{
		if (AbstractModelEntry *entry = createEntry(row))
		{
			newEntries.push_back(entry);
		}
	}
	count = static_cast<int>(newEntries.size());
	if (!count) return false;

	beginInsertRows(parent, row, row + count - 1);
	m_entries.insert(m_entries.begin() + row, newEntries.begin(), newEntries.end());
	endInsertRows();
	return true;
}

bool AbstractListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid()) return false; // Only remove rows from root
	int startRow = std::max(row, 0);
	int endRow = std::min(row + count, rowCount(parent) - 1);
	if (endRow <= startRow) return false; // Nothing to remove
	beginRemoveRows(parent, startRow, endRow);
	for (auto it = m_entries.begin() + startRow, end = m_entries.begin() + endRow + 1; it != end; ++it)
	{
		// todo: handle cases where destroyEvent returns false
		destroyEntry(*it);
	}
	m_entries.erase(m_entries.begin() + startRow, m_entries.begin() + endRow);
	endRemoveRows();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Drag and drop

Qt::DropActions AbstractListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

///////////////////////////////////////////////////////////////////////////////
// Utils

bool AbstractListModel::isIndexValid(int row, int column, const QModelIndex &parent) const {
	return column >= 0 && column < columnCount(parent)
		&& row >= 0 && row < rowCount(parent);
}
bool AbstractListModel::isIndexValid(const QModelIndex &index) const {
	return index.isValid() && isIndexValid(index.row(), index.column(), index.parent());
}

int AbstractListModel::columnToRole(int column) const
{
	return InvalidRole;
}
