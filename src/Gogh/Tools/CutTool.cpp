#include "CutTool.h"
#include "Logger.h"

#include <QGraphicsView>
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
		
	}
}

void CutTool::finish(QPoint position)
{
	update(position);
	setActive(false);
	m_cutShape = QPainterPath();
}
