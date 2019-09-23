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

#include "AbstractListModel.h"
#include "Parameter.h"
#include "Graph.h"

namespace Gogh {
namespace Gui {

/**
 * List of parameters attached to a node
 */
class ParameterListModel : public AbstractListModel
{
	Q_OBJECT
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

public:
	void setNode(NodePtr node);

public:
	// Basic QAbstractItemModel implementation
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	// Editable QAbstractItemModel implementation
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	
	// Headers QAbstractItemModel implementation
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// QML Roles
	QHash<int, QByteArray> roleNames() const override;

protected:
	// Implementation of AbstractListModel
	AbstractModelEntry * createEntry(int row) override;
	bool destroyEntry(int row, AbstractModelEntry * entry) override;

	int columnToRole(int column) const override;

private:
	class ParameterModel : public AbstractModelEntry {
	public:
		std::shared_ptr<Parameter> m_parameter;

	public:
		ParameterModel(std::shared_ptr<Parameter> parameter) : m_parameter(parameter) {}

		QVariant data(int role) const override;
		bool setData(int role, QVariant value) override;

	private:
		// Static utils, that may eventually be more suited to another location but
		// for now stay here because this class is the only one to use them
		static bool setParamFromQVariant(std::shared_ptr<Parameter> param, const QVariant &value);
	};

	void reloadFromNode() noexcept;

private:
	NodePtr m_node;
};

} // namespace Gui
} // namespace Gogh

Q_DECLARE_METATYPE(std::shared_ptr<Gogh::Gui::ParameterListModel>)

#endif // H_GOGH_PARAMETERLISTMODEL
