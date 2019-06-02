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

#include "ParameterListModel.h"
#include "Logger.h"

using namespace Gogh::Gui;

ParameterListModel::ParameterListModel()
{
	// DEBUG
	auto p = std::make_shared<Parameter>();
	p->setName("Param_1");
	m_parameters.push_back(p);
	p = std::make_shared<Parameter>();
	p->setName("Param_2");
	m_parameters.push_back(p);
}

///////////////////////////////////////////////////////////////////////////////
// Basic QAbstractItemModel implementation

QModelIndex ParameterListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column < 0 || column >= columnCount(parent)) return QModelIndex();
	if (parent.isValid()) return QModelIndex(); // Children don't have sub-children
	if (row < 0 || row >= rowCount(parent)) return QModelIndex(); // out of bounds
	return createIndex(row, column, nullptr);
}

QModelIndex ParameterListModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int ParameterListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have subchildren
	return static_cast<int>(m_parameters.size());
}

int ParameterListModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have subchildren
	return _ColumnCount;
}

QVariant ParameterListModel::data(const QModelIndex &index, int role) const
{
	if ((role == Qt::DisplayRole || role == Qt::EditRole) && index.isValid()
		&& index.column() >= 0 && index.column() < columnCount(index.parent())
		&& index.row() >= 0 && index.row() < rowCount(index.parent()))
	{
		switch (index.column())
		{
		case NameColumn:
			return QString::fromStdString(m_parameters[index.row()]->name());
		case TypeColumn:
		{
			ParameterType type = m_parameters[index.row()]->type();
			return role == Qt::EditRole
				? QVariant(type)
				: QVariant(QString::fromStdString(ParameterTypeUtils::parameterTypeName(type)));
		}
		case ValueColumn:
			if (role == Qt::EditRole)
			{
				std::shared_ptr<Parameter> param = m_parameters[index.row()];
				switch (param->type()) {
				case NoneType:
					return QVariant();
				case StringType:
				case ButtonType:
					return QString::fromStdString(param->rawValue().toString());
				case IntType:
				case EnumType:
					return param->rawValue().toInt();
				case CheckboxType:
					return param->rawValue().toBool();
				}
			}
			return QString::fromStdString(m_parameters[index.row()]->evalAsString());
		default:
			return QVariant();
		}
	}
	return QVariant();
}

///////////////////////////////////////////////////////////////////////////////
// Editable QAbstractItemModel implementation

bool ParameterListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole && index.isValid()
		&& index.column() >= 0 && index.column() < columnCount(index.parent())
		&& index.row() >= 0 && index.row() < rowCount(index.parent()))
	{
		switch (index.column())
		{
		case NameColumn:
			m_parameters[index.row()]->setName(value.toString().toStdString());
			dataChanged(index, index);
			return true;
		case TypeColumn:
		{
			std::shared_ptr<Parameter> param = m_parameters[index.row()];
			const QModelIndex & valueIndex = index.siblingAtColumn(ValueColumn);
			// Get value before changing type, then write it to the new typed param
			QVariant prevValue = valueIndex.data(Qt::EditRole);
			param->setType(ParameterTypeUtils::fromInt(value.toInt()));
			setParamFromQVariant(param, prevValue);
			dataChanged(index, valueIndex);
			return true;
		}
		case ValueColumn:
		{
			if (setParamFromQVariant(m_parameters[index.row()], value)) {
				dataChanged(index, index);
				return true;
			}
			else {
				return false;
			}
		}
		default:
			return false;
		}
	}
	return false;
}

Qt::ItemFlags ParameterListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags itemFlags = QAbstractItemModel::flags(index);

	if (index.column() == NameColumn || index.column() == TypeColumn || index.column() == ValueColumn) {
		if (index.column() == ValueColumn
			&& index.column() >= 0 && index.column() < columnCount(index.parent())
			&& index.row() >= 0 && index.row() < rowCount(index.parent())
			&& m_parameters[index.row()]->type() == NoneType
		)
		{
			// Value is not editable if type is NoneType
		}
		else
		{
			itemFlags |= Qt::ItemIsEditable;
		}
	}

	if (index.isValid())
	{
		itemFlags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	}

	return itemFlags;
}

///////////////////////////////////////////////////////////////////////////////
// Headers QAbstractItemModel implementation

QVariant ParameterListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) return QVariant();

	switch (section)
	{
	case NameColumn:
		return "Name";
	case TypeColumn:
		return "Type";
	case ValueColumn:
		return "Value";
	default:
		return QVariant();
	}
}

///////////////////////////////////////////////////////////////////////////////
// Resizable QAbstractItemModel implementation

bool ParameterListModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid()) return false; // Only insert rows at root
	if (row < 0 || row > rowCount(parent)) return false;

	beginInsertRows(parent, row, row + count - 1);
	m_parameters.insert(m_parameters.begin() + row, count, nullptr);
	for (int i = row; i < row + count; ++i)
	{
		m_parameters[i] = std::make_shared<Parameter>();
	}
	endInsertRows();
	return true;
}

bool ParameterListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid()) return false; // Only remove rows from root
	int startRow = std::max(row, 0);
	int endRow = std::min(row + count, rowCount(parent) - 1);
	if (endRow <= startRow) return false; // Nothing to remove
	beginRemoveRows(parent, startRow, endRow);
	m_parameters.erase(m_parameters.begin() + startRow, m_parameters.begin() + endRow);
	endRemoveRows();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Drag and drop

Qt::DropActions ParameterListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

///////////////////////////////////////////////////////////////////////////////
// Static utils

bool ParameterListModel::setParamFromQVariant(std::shared_ptr<Gogh::Parameter> param, const QVariant &value)
{
	switch (param->type()) {
	case NoneType:
		return false;
	case StringType:
	case ButtonType:
		param->set(value.toString().toStdString());
		break;
	case IntType:
	case EnumType:
		param->set(value.toInt());
		break;
	case CheckboxType:
		param->set(value.toBool());
		break;
	}
	return true;
}