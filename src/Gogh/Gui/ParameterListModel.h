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

#ifndef H_GOGH_PARAMETERLISTMODEL
#define H_GOGH_PARAMETERLISTMODEL

#include <QAbstractItemModel>
#include <QModelIndex>

#include "Parameter.h"
#include "Graph.h"

namespace Gogh {
namespace Gui {

/**
 * List of parameters attached to a node
 */
class ParameterListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	void setNode(NodePtr node);

public:
	// Basic QAbstractItemModel implementation
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	// Editable QAbstractItemModel implementation
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	// Headers QAbstractItemModel implementation
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Resizable QAbstractItemModel implementation
	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

	// Drag and drop
	Qt::DropActions supportedDropActions() const override;

	// QML Roles
	QHash<int, QByteArray> roleNames() const override;

public:
	enum Column {
		NameColumn = 0,
		TypeColumn,
		ValueColumn,
		_ColumnCount,
	};

	enum Role {
		InvalidRole = Qt::UserRole,
		NameRole,
		TypeRole,
		ValueRole,
	};

private:
	// Static utils, that may eventually be more suited to another location but
	// for now stay here because this class is the only one to use them
	static bool setParamFromQVariant(std::shared_ptr<Parameter> param, const QVariant &value);

	/**
	 * The same row data can be accessed both as a role and as a column.
	 * It is the role that is the primary way of accessing it, but column is
	 * useful for debugging, to be able to quickly display the model in a tree
	 * view (in an of outline-like interface), so this functions makes the link
	 * between the two.
	 */
	static Role columnToRole(int column);

private:
	NodePtr m_node;
};

} // namespace Gui
} // namespace Gogh

Q_DECLARE_METATYPE(std::shared_ptr<Gogh::Gui::ParameterListModel>)

#endif // H_GOGH_PARAMETERLISTMODEL
