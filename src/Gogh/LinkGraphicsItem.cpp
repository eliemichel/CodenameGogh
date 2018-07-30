#include "LinkGraphicsItem.h"

#include <QPainter>

#include <cmath>

void LinkGraphicsItem::setStartPos(QPointF pos) {
	prepareGeometryChange();
	m_startPos = pos;
}

void LinkGraphicsItem::setEndPos(QPointF pos) {
	prepareGeometryChange();
	m_endPos = pos;
}

QRectF LinkGraphicsItem::boundingRect() const
{
	float penWidth = 1.f;
	float w = std::abs(m_startPos.x() - m_endPos.x());
	float h = std::abs(m_startPos.y() - m_endPos.y());
	float minx = std::min(m_startPos.x(), m_endPos.x());
	float miny = std::min(m_startPos.y(), m_endPos.y());
	return QRectF(minx - penWidth / 2, miny - penWidth / 2, w + penWidth, h + penWidth);
}

void LinkGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setPen(QPen(QColor(192, 192, 192), 1));
	float dx = m_endPos.x() - m_startPos.x();
	float dy = m_endPos.y() - m_startPos.y();
	float x = m_startPos.x();
	float y = m_startPos.y();
	int n = std::max(std::abs(dx), std::abs(dy)) / 2;
	QPointF *points = new QPointF[n + 1];
	for (int i = 0; i <= n; ++i)
	{
		float t = i / (float)n;
		points[i].setX(x + t * dx);
		points[i].setY(y + (1.f - cos(t * 3.141593)) / 2 * dy);
	}
	painter->drawPolyline(points, n + 1);
	delete[] points;
}

bool LinkGraphicsItem::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
	return false;
}

QPainterPath LinkGraphicsItem::shape() const
{
	QPainterPath path;
	return path;
}
