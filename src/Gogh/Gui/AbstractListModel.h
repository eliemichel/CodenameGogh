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

#ifndef H_GOGH_ABSTRACTLISTMODEL
#define H_GOGH_ABSTRACTLISTMODEL

#include <vector>
#include <QAbstractItemModel>

namespace Gogh {
namespace Gui {

/**
 * List of nodes in a graph.
 * This is a proxy to an underlying graph model stored in a Graph object.
 * Before doing anything with this, you must call setGraph() with a valid graph
 * pointer.
 * The models returned when asking for parameters, inputs or outputs are
 * themselves wrappers around the underlying graph elements. Those wrappers add
 * standard Qt model API to non-qt core classes like Parameter, Input and
 * Output.
 */
class AbstractListModel : public QAbstractItemModel
{
public:
	/**
	 * Redefine in subclasses to complete with your own roles
	 */
	enum Role {
		InvalidRole = Qt::UserRole,
	};
public:
	// Basic QAbstractItemModel implementation
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	// Editable QAbstractItemModel implementation
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	// Resizable QAbstractItemModel implementation
	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

	// Drag and drop
	Qt::DropActions supportedDropActions() const override;

protected:
	/**
	 * Structure holding the node pointer together with the qmodels for its
	 * parameters, inputs and outputs. This is used to avoid populating the
	 * core graph structures with qt-related (so ui-related) attributes.
	 */
	class AbstractModelEntry {
	public:
		virtual QVariant data(int role) const = 0;
		virtual bool setData(int role, QVariant value) = 0;
	};

protected:
	// Utils
	
	/**
	 * Check that index is in bounds
	 */
	bool isIndexValid(int row, int column, const QModelIndex &parent) const;
	bool isIndexValid(const QModelIndex &index) const;

	/**
	 * Create a new model entry
	 */
	virtual AbstractModelEntry * createEntry(int row) = 0;
	virtual bool destroyEntry(AbstractModelEntry * entry) = 0;
	
	/**
	 * The same row data can be accessed both as a role and as a column.
	 * It is the role that is the primary way of accessing it, but column is
	 * useful for debugging, to be able to quickly display the model in a tree
	 * view (in an of outline-like interface), so this functions makes the link
	 * between the two.
	 */
	virtual int columnToRole(int column) const;

protected:
	// Static utils

	/**
	 * This is an alias to "is not valid" used for clearer reading
	 */
	static inline bool isRoot(const QModelIndex & index) { return !index.isValid(); }

protected:
	/**
	 * NEVER holds null values
	 * Try to avoid using this directly in subclasses, but it is available anyway in case you need
	 */
	std::vector<AbstractModelEntry*> m_entries;
};

} // namespace Gui
} // namespace Gogh

#endif // H_GOGH_ABSTRACTLISTMODEL
