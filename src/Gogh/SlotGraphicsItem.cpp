#include "SlotGraphicsItem.h"
#include "NodeGraphView.h"
#include "LinkGraphicsItem.h"
#include "Slot.h"

#include <QGraphicsSceneMouseEvent>

SlotGraphicsItem::SlotGraphicsItem(QGraphicsItem *parent)
	: QGraphicsEllipseItem(0, 0, 15, 15, parent)
	, m_slot(nullptr)
{
	setData(NodeGraphView::RoleData, NodeGraphView::SlotRole);
	setPen(QPen(Qt::black));
	setBrush(QBrush(QColor(215, 192, 128)));
}

void SlotGraphicsItem::updateLinks() const
{
	if (!slot())
	{
		return;
	}

	for (LinkGraphicsItem *l : slot()->inputLinks())
	{
		l->setEndPos(sceneBoundingRect().center());
		l->update();
	}
	for (LinkGraphicsItem *l : slot()->outputLinks())
	{
		l->setStartPos(sceneBoundingRect().center());
		l->update();
	}
}


void SlotGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}