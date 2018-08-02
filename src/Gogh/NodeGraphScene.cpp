#include "NodeGraphScene.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphView.h"

#include <QModelIndex>

NodeGraphicsItem * NodeGraphScene::toNodeItem(QGraphicsItem *item) const
{
	QVariant v = item->data(NodeGraphView::RoleData);
	if (v.isValid() && v.toInt() == NodeGraphView::NodeControlRole)
	{
		NodeGraphicsItem *nodeItem = item->data(NodeGraphView::NodePointerData).value<NodeGraphicsItem*>();
		return nodeItem;
	}
	else
	{
		return nullptr;
	}
}

NodeGraphicsItem * NodeGraphScene::nodeItemAtIndex(const QModelIndex & index)
{
	// TODO: find a proper data structure
	for (QGraphicsItem *item : items())
	{
		if (NodeGraphicsItem *nodeItem = toNodeItem(item))
		{
			if (nodeItem->modelIndex() == index)
			{
				return nodeItem;
			}
		}
	}
	return nullptr;
}
