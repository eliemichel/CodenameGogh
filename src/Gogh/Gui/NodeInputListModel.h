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

#ifndef H_GOGH_NODEINPUTLISTMODEL
#define H_GOGH_NODEINPUTLISTMODEL

#include <QAbstractItemModel>
#include <QModelIndex>

#include"AbstractListModel.h"
#include "Graph.h"
#include "ParameterListModel.h"

namespace Gogh {
namespace Gui {

/**
 * List of inputs attached to a node
 * TODO: manage input type
 */
class NodeInputListModel : public AbstractListModel
{
	Q_OBJECT
public:
	enum Columns {
		NameColumn = 0,
		TypeColumn,
		ViewXColumn,
		ViewYColumn,
		_ColumnCount,
	};

	enum Role {
		InvalidRole = Qt::UserRole,
		NameRole,
		TypeRole,
		ViewXRole,
		ViewYRole,
	};

public:
	void setNode(NodePtr node);

public:
	// Basic QAbstractTableModel implementation
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	// Headers QAbstractItemModel implementation
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

protected:
	// Implementation of AbstractListModel
	AbstractModelEntry * createEntry(int row) override;
	bool destroyEntry(int row, AbstractModelEntry * entry) override;

	int columnToRole(int column) const override;

private:
	void reloadFromNode() noexcept;

private:
	class NodeInputModel : public AbstractModelEntry {
	public:
		NodeInputModel(std::shared_ptr<NodeInput> input) : m_input(input) {}

		QVariant data(int role) const override;
		bool setData(int role, QVariant value) override;

	private:
		std::shared_ptr<NodeInput> m_input;
		// Position of the slot, written by the node delegates, read by the
		// edge delegates
		float m_x;
		float m_y;
	};

private:
	NodePtr m_node;
};

} // namespace Gui
} // namespace Gogh

Q_DECLARE_METATYPE(std::shared_ptr<Gogh::Gui::NodeInputListModel>)

#endif // H_GOGH_NODEINPUTLISTMODEL
