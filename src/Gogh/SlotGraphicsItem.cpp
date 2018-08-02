#include "SlotGraphicsItem.h"
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
	setData(NodeGraphScene::RoleData, NodeGraphScene::SlotRole);
	setPen(QPen(Qt::black));
	setBrush(QBrush(QColor(99, 99, 199)));
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
		if (slot() && slot()->sourceSlot() && slot()->sourceSlot()->graphicItem())
		{
			slot()->sourceSlot()->graphicItem()->removeOutputLink(m_inputLink);
		}
		scene()->removeItem(m_inputLink);
		delete m_inputLink;
	}
	m_inputLink = link;
}

void SlotGraphicsItem::removeOutputLink(LinkGraphicsItem *link)
{
	std::vector<LinkGraphicsItem*> newOutputs;
	newOutputs.reserve(m_outputLinks.size());
	for (LinkGraphicsItem *l : m_outputLinks)
	{
		if (l != link)
		{
			newOutputs.push_back(l);
		}
	}
	m_outputLinks = newOutputs;
}

void SlotGraphicsItem::updateLinks() const
{
	if (inputLink())
	{
		inputLink()->setEndPos(sceneBoundingRect().center());
		inputLink()->update();
	}
	for (LinkGraphicsItem *l : outputLinks())
	{
		l->setStartPos(sceneBoundingRect().center());
		l->update();
	}
}

