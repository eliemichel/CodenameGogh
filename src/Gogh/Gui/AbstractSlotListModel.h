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

#ifndef H_GOGH_ABSTRACTSLOTLISTMODEL
#define H_GOGH_ABSTRACTSLOTLISTMODEL

#include <QAbstractItemModel>
#include <QModelIndex>

#include "Graph.h"
#include "ParameterListModel.h"

namespace Gogh {
namespace Gui {

/**
* List of inputs attached to a node
*/
class AbstractSlotListModel : public QAbstractTableModel
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

protected:
	// Methods to be implemented in subclasses, to make the difference between
	// input and output slots
	virtual std::vector<NodeInputPtr> & slotList() = 0;
	virtual const std::vector<NodeInputPtr> & slotList() const = 0;
	virtual NodeInputPtr makeSlot() const = 0;

	// Utility accessor to help implementing virtual methods
	inline NodePtr modelNode() const { return m_node; }

public:
	enum Columns {
		NameColumn = 0,
		TypeColumn,
		_ColumnCount,
	};

private:
	NodePtr m_node;
};

} // namespace Gui
} // namespace Gogh

Q_DECLARE_METATYPE(std::shared_ptr<Gogh::Gui::AbstractSlotListModel>)

#endif // H_GOGH_ABSTRACTSLOTLISTMODEL
