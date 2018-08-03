#include "LinkGraphicsItem.h"
#include "NodeGraphScene.h"

#include <QPainter>
#include <QPainterPath>

#include <cmath>
#include <algorithm>

LinkGraphicsItem::LinkGraphicsItem(QGraphicsItem *parent)
	: QGraphicsItem(parent)
	, m_endSlotItem(nullptr)
{
	setData(NodeGraphScene::RoleData, NodeGraphScene::LinkRole);
	setZValue(NodeGraphScene::LinkLayer);
}

void LinkGraphicsItem::setStartPos(QPointF pos) {
	m_startPos = pos;
	updateShape();
}

void LinkGraphicsItem::setEndPos(QPointF pos) {
	m_endPos = pos;
	updateShape();
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
	painter->drawPath(m_shape);
}

QPainterPath LinkGraphicsItem::shape() const
{
	return m_shape;
}

void LinkGraphicsItem::updateShape()
{
	prepareGeometryChange();
	m_shape = QPainterPath();
	m_shape.moveTo(m_startPos);
	double dx = std::max(-200.0, m_endPos.x() - m_startPos.x());
	double lx = std::max(50.0, std::abs(dx));
	QPointF a(m_startPos.x() + lx * 0.5, m_startPos.y());
	QPointF b(m_endPos.x() - lx * 0.5, m_endPos.y());
	m_shape.cubicTo(a, b, m_endPos);
}
