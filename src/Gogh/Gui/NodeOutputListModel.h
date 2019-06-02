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

#ifndef H_GOGH_NODEOUTPUTLISTMODEL
#define H_GOGH_NODEOUTPUTLISTMODEL

#include <QAbstractItemModel>
#include <QModelIndex>

#include "Graph.h"
#include "ParameterListModel.h"

namespace Gogh {
namespace Gui {

/**
 * List of outputs attached to a node
 * A lot of code duplication with the very similar NodeInputListModel, but I
 * could not find a clean way of factorizing it, especially because Q_OBJECT
 * do not support templating
 */
class NodeOutputListModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	void setNode(NodePtr node);

public:
	// Basic QAbstractTableModel implementation
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	// Editable QAbstractTableModel implementation
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	// Headers QAbstractItemModel implementation
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Resizable QAbstractTableModel implementation
	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

	// Drag and drop
	Qt::DropActions supportedDropActions() const override;

public:
	enum Columns {
		NameColumn = 0,
		TypeColumn,
		ViewXColumn,
		ViewYColumn,
		_ColumnCount,
	};

private:
	/**
	 * Data related to the view, so not serialized and not stored in the
	 * backend object.
	 * Beware to keep it in sync with the backend outputs
	 */
	struct ViewDataEntry {
		// Position of the slot, written by the node delegates, read by the
		// edge delegates
		float x;
		float y;
	};
	std::vector<ViewDataEntry> m_viewData;
	NodePtr m_node;
};

} // namespace Gui
} // namespace Gogh

Q_DECLARE_METATYPE(std::shared_ptr<Gogh::Gui::NodeOutputListModel>)

#endif // H_GOGH_NODEOUTPUTLISTMODEL
