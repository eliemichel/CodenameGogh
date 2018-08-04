#include "CutTool.h"

#include "NodeGraphView.h"
#include "NodeGraphScene.h"
#include "SlotGraphicsItem.h"
#include "LinkGraphicsItem.h"
#include "Logger.h"

#include <cmath>

void CutTool::start(QPoint position)
{
	setActive(true);
	m_cutShape = QPainterPath();
	m_cutShape.moveTo(view()->mapToScene(position));
}

void CutTool::update(QPoint position)
{
	QPointF oldPos = m_cutShape.currentPosition();
	QPointF curPos = view()->mapToScene(position);
	m_cutShape.lineTo(curPos);
	QPointF topLeft(std::min(curPos.x(), oldPos.x()), std::min(curPos.y(), oldPos.y()));
	QSizeF size(std::abs(curPos.x() - oldPos.x()), std::abs(curPos.y() - oldPos.y()));
	view()->scene()->update(QRectF(topLeft, size));

	// test collision
	QPainterPath edge;
	edge.moveTo(oldPos);
	edge.lineTo(curPos);
	QList<QGraphicsItem*> items = view()->scene()->items(edge);
	for (QGraphicsItem *item : items)
	{
		if (LinkGraphicsItem *linkItem = view()->nodeGraphScene()->toLinkItem(item))
		{
			if (SlotGraphicsItem *slotItem = linkItem->endSlotItem())
			{
				DEBUG_LOG << "remove link";
				slotItem->setInputLink(nullptr);
			}
			else
			{
				view()->scene()->removeItem(item);
				delete item;
			}
		}
	}
}

void CutTool::finish(QPoint position)
{
	update(position);
	setActive(false);
	m_cutShape = QPainterPath();
}
