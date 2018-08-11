#include "SlotGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphScene.h"
#include "LinkGraphicsItem.h"
#include "Logger.h"

#include <QGraphicsSceneMouseEvent>

SlotGraphicsItem::SlotGraphicsItem(QGraphicsItem *parent)
	: QGraphicsEllipseItem(0, 0, 15, 15, parent)
	, m_inputLink(nullptr)
{
	setPen(QPen(Qt::black));
	setBrush(QBrush(QColor(99, 99, 199)));

	setData(NodeGraphScene::RoleData, NodeGraphScene::SlotRole);
	setZValue(NodeGraphScene::SlotLayer);
}

void SlotGraphicsItem::removeInputLink()
{
	if (m_inputLink)
	{
		scene()->removeItem(m_inputLink);
		delete m_inputLink;
	}
	m_inputLink = nullptr;
}

void SlotGraphicsItem::ensureInputLink()
{
	if (!m_inputLink)
	{
		m_inputLink = new LinkGraphicsItem();
		m_inputLink->setEndSlotItem(this);
		m_inputLink->setEndPos(slotCenter());
		scene()->addItem(m_inputLink);
	}
}
