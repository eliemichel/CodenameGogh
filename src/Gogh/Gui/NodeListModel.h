/**
* This file is part of Gogh.
* It is released under the terms of the MIT License:
*
* Copyright (c) 2018 - 2019 -- �lie Michel <elie.michel@exppad.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the �Software�), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* The Software is provided �as is�, without warranty of any kind, express or
* implied, including but not limited to the warranties of merchantability,
* fitness for a particular purpose and non-infringement. In no event shall the
* authors or copyright holders be liable for any claim, damages or other
* liability, whether in an action of contract, tort or otherwise, arising
* from, out of or in connection with the software or the use or other dealings
* in the Software.
*/

#ifndef H_GOGH_NODELISTMODEL
#define H_GOGH_NODELISTMODEL

#include <QAbstractItemModel>
#include <QModelIndex>

#include "Graph.h"
#include "ParameterListModel.h"
#include "NodeInputListModel.h"

namespace Gogh {
namespace Gui {

/**
 * List of nodes in a graph
 */
class NodeListModel : public QAbstractItemModel
{
public:
	NodeListModel();

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

private:
	// Utils
	/// This is an alias to "is not valid" used for clearer reading
	static inline bool isRoot(const QModelIndex & index) { return !index.isValid(); }

public:
	enum Columns {
		NameColumn = 0,
		XPosColumn,
		YPosColumn,
		_ColumnCount,
	};

	enum Roles {
		NodePtrRole = Qt::UserRole,
		ParameterModelRole,
		InputModelRole,
	};

private:
	/**
	 * Structure holding the node pointer together with the qmodels for its
	 * parameters, inputs and outputs. This is used to avoid populating the
	 * core graph structures with qt-related (so ui-related) attributes.
	 */
	struct ModelEntry {
		NodePtr node;
		std::shared_ptr<ParameterListModel> parameters;
		std::shared_ptr<NodeInputListModel> inputs;

		ModelEntry(NodePtr _node)
		{
			node = _node;
			parameters = std::make_shared<ParameterListModel>();
			parameters->setNode(node);
			inputs = std::make_shared<NodeInputListModel>();
			inputs->setNode(node);
		}
	};
	std::vector<ModelEntry> m_entries;
};

} // namespace Gui
} // namespace Gogh

#endif // H_GOGH_NODELISTMODEL
