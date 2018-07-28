#include "NodeGraphicsItem.h"
#include "LinkGraphicsItem.h"
#include "NodeWidget.h"
#include "Logger.h"
#include "NodeGraphView.h"
#include "SlotGraphicsItem.h"

#include <QWidget>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>

NodeGraphicsItem::NodeGraphicsItem(QGraphicsScene *scene, NodeWidget *content)
	: m_content(content)
{
	content->resize(content->sizeHint());

	scene->addItem(this);
	setRect(0, 0, content->width(), 20);
	setPen(QPen(Qt::black));
	setBrush(QBrush(Qt::darkBlue));
	setFlag(ItemIsMovable, true);
	setFlag(ItemIsSelectable, true);
	setFlag(ItemSendsGeometryChanges, true);

	m_proxy = scene->addWidget(content);
	m_proxy->setData(NodeGraphView::RoleData, NodeGraphView::NodeContentRole);
	m_proxy->setPos(0, rect().height());
	m_proxy->setParentItem(this);

	// Wrap a SlotGraphiscItem around all the slots of the content node
	int offset = 30;
	for (Slot *s : content->inputSlots())
	{
		SlotGraphicsItem *slotItem = new SlotGraphicsItem();
		scene->addItem(slotItem);
		slotItem->setSlot(s);
		slotItem->setPos(-15, offset);
		slotItem->setParentItem(this);
		m_slotItems.push_back(slotItem);
		offset += 30;
	}
	offset = 30;
	for (Slot *s : content->outputSlots())
	{
		SlotGraphicsItem *slotItem = new SlotGraphicsItem();
		scene->addItem(slotItem);
		slotItem->setSlot(s);
		slotItem->setPos(rect().width(), offset);
		slotItem->setParentItem(this);
		m_slotItems.push_back(slotItem);
		offset += 30;
	}
}

QVariant NodeGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionChange && scene()) {
		for (SlotGraphicsItem *item : m_slotItems)
		{
			item->updateLinks();
		}
	}
	return QGraphicsItem::itemChange(change, value);
}