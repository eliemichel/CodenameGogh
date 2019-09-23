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

#ifndef H_GOGH_NODELISTMODEL
#define H_GOGH_NODELISTMODEL

#include <QAbstractItemModel>
#include <QModelIndex>

#include "Graph.h"
#include "AbstractListModel.h"
#include "ParameterListModel.h"
#include "NodeInputListModel.h"
#include "NodeOutputListModel.h"

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
class NodeListModel : public AbstractListModel
{
public:
	/**
	 * Name of the model's columns
	 */
	enum Columns {
		NameColumn = 0,
		XPosColumn,
		YPosColumn,
		_ColumnCount,
	};

	/**
	 * Name of the model's custom roles. Roles are the primary way of accessing
	 * to model's data. Some of them are also accessible as column, mostly to
	 * ease the integration to standard tree view for debug interfaces like
	 * outliner.
	 */
	enum Role {
		InvalidRole = Qt::UserRole,
		NodePtrRole,
		NameRole,
		XPosRole,
		YPosRole,
		ParameterModelRole,
		InputModelRole,
		OutputModelRole,
		// Return raw pointers instead of shared pointers (for qml binding)
		RawParameterModelRole,
		RawInputModelRole,
		RawOutputModelRole,
	};

public:
	/**
	 * Set the underlying graph model.
	 * You can call it again with another graph to change the target of this
	 * model.
	 * @param graph: MUST point to a valid graph
	 */
	void setGraph(GraphPtr graph);

public:
	// Basic QAbstractItemModel implementation
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	// Headers QAbstractItemModel implementation
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// QML Roles
	QHash<int, QByteArray> roleNames() const override;

protected:
	// Utils
	
	AbstractModelEntry * createEntry(int row) override;
	bool destroyEntry(int row, AbstractModelEntry *entry) override;

	int columnToRole(int column) const override;

private:
	/**
	 * Structure holding the node pointer together with the qmodels for its
	 * parameters, inputs and outputs. This is used to avoid populating the
	 * core graph structures with qt-related (so ui-related) attributes.
	 */
	struct ModelEntry : public AbstractModelEntry {
		NodePtr node;
		std::shared_ptr<ParameterListModel> parametersModel;
		std::shared_ptr<NodeInputListModel> inputsModel;
		std::shared_ptr<NodeOutputListModel> outputsModel;

		ModelEntry(NodePtr _node)
		{
			node = _node;
			parametersModel = std::make_shared<ParameterListModel>();
			parametersModel->setNode(node);
			inputsModel = std::make_shared<NodeInputListModel>();
			inputsModel->setNode(node);
			outputsModel = std::make_shared<NodeOutputListModel>();
			outputsModel->setNode(node);
		}

		QVariant data(int role) const override;
		bool setData(int role, QVariant value) override;
	};

	/**
	* Load the ModelEntries from the current m_graph
	* You are likely to need to wrap this inside a beginResetModel() block
	*/
	void reloadFromGraph() noexcept;

private:
	//std::vector<ModelEntry> m_entries;
	GraphPtr m_graph;
};

} // namespace Gui
} // namespace Gogh

#endif // H_GOGH_NODELISTMODEL
