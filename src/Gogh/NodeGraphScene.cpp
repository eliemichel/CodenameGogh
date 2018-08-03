#include "NodeGraphScene.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphView.h"
#include "SlotGraphicsItem.h"
#include "LinkGraphicsItem.h"

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
