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

void ParameterListModel::setNode(Gogh::NodePtr node)
{
	beginResetModel();
	m_node = node;
	reloadFromNode();
	endResetModel();
}

///////////////////////////////////////////////////////////////////////////////
// Basic QAbstractItemModel implementation

int ParameterListModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0; // Children don't have subchildren
	return _ColumnCount;
}

///////////////////////////////////////////////////////////////////////////////
// Editable QAbstractItemModel implementation

Qt::ItemFlags ParameterListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags itemFlags = QAbstractItemModel::flags(index);

	if (index.column() == NameColumn || index.column() == TypeColumn || index.column() == ValueColumn) {
		if (index.column() == ValueColumn
			&& index.column() >= 0 && index.column() < columnCount(index.parent())
			&& index.row() >= 0 && index.row() < rowCount(index.parent())
			&& m_node->parameters[index.row()]->type() == NoneType
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
// QML Roles

QHash<int, QByteArray> ParameterListModel::roleNames() const
{
	QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
	names[NameRole] = "name";
	names[TypeRole] = "type";
	names[ValueRole] = "value";
	return names;
}

///////////////////////////////////////////////////////////////////////////////
// Implementation of AbstractListModel

ParameterListModel::AbstractModelEntry * ParameterListModel::createEntry(int row)
{
	m_node->parameters.insert(m_node->parameters.begin() + row, 1, nullptr);
	m_node->parameters[row] = std::make_shared<Parameter>();
	return new ParameterModel(m_node->parameters[row]);
}

bool ParameterListModel::destroyEntry(int row, AbstractModelEntry * entry)
{
	if (row >= m_node->parameters.size()) return false;
	ParameterModel* parameterModel = static_cast<ParameterModel*>(entry);
	m_node->parameters.erase(m_node->parameters.begin() + row);
	delete parameterModel;
	return true;
}

int ParameterListModel::columnToRole(int column) const
{
	switch (column)
	{
	case NameColumn:
		return NameRole;
	case TypeColumn:
		return TypeRole;
	case ValueColumn:
		return ValueRole;
	default:
		return InvalidRole;
	}
}

///////////////////////////////////////////////////////////////////////////////
// ModelEntry methods

QVariant ParameterListModel::ParameterModel::data(int role) const {
	switch (role)
	{
	case NameRole:
		return QString::fromStdString(m_parameter->name());

	case TypeRole:
	{
		ParameterType type = m_parameter->type();
		return true //role == Qt::EditRole // TODO: distinguish edit from read only cases
			? QVariant(type)
			: QVariant(QString::fromStdString(ParameterTypeUtils::parameterTypeName(type)));
	}

	case ValueRole:
		if (true /*role == Qt::EditRole*/)
		{
			switch (m_parameter->type()) {
			case NoneType:
				return QVariant();
			case StringType:
			case ButtonType:
				return QString::fromStdString(m_parameter->rawValue().toString());
			case IntType:
			case EnumType:
				return m_parameter->rawValue().toInt();
			case CheckboxType:
				return m_parameter->rawValue().toBool();
			}
		}
		return QString::fromStdString(m_parameter->evalAsString());

	default:
		return QVariant();
	}
}

bool ParameterListModel::ParameterModel::setData(int role, QVariant value) {
	switch (role)
	{
	case NameRole:
		m_parameter->setName(value.toString().toStdString());
		return true;

	case TypeRole:
	{
		// Get value before changing type, then write it to the new typed param
		QVariant prevValue = data(ValueRole);
		m_parameter->setType(ParameterTypeUtils::fromInt(value.toInt()));
		setData(ValueRole, prevValue);
		return true;
	}

	case ValueRole:
		return setParamFromQVariant(m_parameter, value);

	default:
		return false;
	}
}

bool ParameterListModel::ParameterModel::setParamFromQVariant(std::shared_ptr<Gogh::Parameter> param, const QVariant &value)
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

void ParameterListModel::reloadFromNode() noexcept
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
	m_entries.reserve(m_node->parameters.size());
	for (int i = 0; i < m_node->parameters.size(); ++i)
	{
		m_entries.push_back(new ParameterModel(m_node->parameters[i]));
	}
}

