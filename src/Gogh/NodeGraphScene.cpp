#include "NodeGraphScene.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphView.h"
#include "SlotGraphicsItem.h"
#include "LinkGraphicsItem.h"
#include "NodeGraphModel.h"

#include <QModelIndex>

NodeGraphicsItem * NodeGraphScene::toNodeItem(QGraphicsItem *item) const
{
	QVariant v = item->data(RoleData);
	if (v.isValid() && v.toInt() == NodeControlRole)
	{
		NodeGraphicsItem *nodeItem = item->data(NodePointerData).value<NodeGraphicsItem*>();
		return nodeItem;
	}
	else
	{
		return nullptr;
	}
}

SlotGraphicsItem * NodeGraphScene::toSlotItem(QGraphicsItem *item) const
{
	QVariant v = item->data(RoleData);
	if (v.isValid() && v.toInt() == SlotRole)
	{
		SlotGraphicsItem *slotItem = static_cast<SlotGraphicsItem*>(item);
		return slotItem;
	}
	else
	{
		return nullptr;
	}
}

LinkGraphicsItem * NodeGraphScene::toLinkItem(QGraphicsItem *item) const
{
	QVariant v = item->data(RoleData);
	if (v.isValid() && v.toInt() == LinkRole)
	{
		LinkGraphicsItem *linkItem = static_cast<LinkGraphicsItem*>(item);
		return linkItem;
	}
	else
	{
		return nullptr;
	}
}

NodeGraphicsItem * NodeGraphScene::nodeItemAtIndex(int index)
{
	// TODO: find a proper data structure
	for (QGraphicsItem *item : items())
	{
		if (NodeGraphicsItem *nodeItem = toNodeItem(item))
		{
			if (nodeItem->node()->nodeIndex() == index)
			{
				return nodeItem;
			}
		}
	}
	return nullptr;
}

void NodeGraphScene::setGraphModel(NodeGraphModel *model)
{
	m_nodeGraphModel = model;
	connect(model, &QAbstractItemModel::dataChanged, this, &NodeGraphScene::onDataChanged);
	connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &NodeGraphScene::onRowsRemoved);

	for (int i = 0; i < model->nodeCount(); ++i)
	{
		Node *node = model->node(i);
		NodeGraphicsItem *nodeItem = new NodeGraphicsItem(this, node);
		nodeItem->setPos(QPointF(node->x, node->y));
		m_nodeItems.push_back(nodeItem);
	}
}

void NodeGraphScene::onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
	for (NodeGraphicsItem *item : m_nodeItems)
	{
		int nodeIndex = item->node()->nodeIndex();
		if (!topLeft.isValid()
			|| !bottomRight.isValid()
			|| (!topLeft.parent().isValid() && !bottomRight.parent().isValid()
				&& nodeIndex >= topLeft.row() && nodeIndex <= bottomRight.row()))
		{
			item->update();
		}
	}
}

void NodeGraphScene::onRowsRemoved(const QModelIndex & parent, int first, int last)
{
	for (int i = 0 ; i < m_nodeItems.size() ; ++i)
	{
		int nodeIndex = m_nodeItems[i]->node()->nodeIndex();
		if (!parent.isValid() && nodeIndex >= first && nodeIndex <= last)
		{
			delete m_nodeItems[i];
			m_nodeItems.erase(m_nodeItems.begin() + i);
			--i;
		}
	}
}
