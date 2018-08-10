#include "SlotGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphScene.h"
#include "LinkGraphicsItem.h"
#include "Slot.h"
#include "Logger.h"

#include <QGraphicsSceneMouseEvent>

SlotGraphicsItem::SlotGraphicsItem(QGraphicsItem *parent)
	: QGraphicsEllipseItem(0, 0, 15, 15, parent)
	, m_slot(nullptr)
	, m_inputLink(nullptr)
{
	setPen(QPen(Qt::black));
	setBrush(QBrush(QColor(99, 99, 199)));

	setData(NodeGraphScene::RoleData, NodeGraphScene::SlotRole);
	setZValue(NodeGraphScene::SlotLayer);
}

void SlotGraphicsItem::setSlot(Slot *slot)
{
	m_slot = slot;
	if (m_slot)
	{
		m_slot->setGraphicItem(this);
	}
}

void SlotGraphicsItem::setInputLink(LinkGraphicsItem *link)
{
	if (m_inputLink)
	{
		scene()->removeItem(m_inputLink);
		delete m_inputLink;
	}
	m_inputLink = link;
}
