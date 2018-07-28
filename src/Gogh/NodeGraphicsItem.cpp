#include "NodeGraphicsItem.h"
#include "LinkGraphicsItem.h"
#include "NodeWidget.h"
#include "Logger.h"

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
	m_proxy->setData(0, 1); // set item as node proxy TODO: use enums
	m_proxy->setPos(0, rect().height());
	m_proxy->setParentItem(this);
}

void NodeGraphicsItem::updateLinks() const
{
	for (Slot *s : m_content->allSlots())
	{
		for (LinkGraphicsItem *l : s->inputLinks())
		{
			l->setEndPos(s->pos() + m_proxy->scenePos());
		}
		for (LinkGraphicsItem *l : s->outputLinks())
		{
			l->setStartPos(s->pos() + m_proxy->scenePos());
		}
	}
}

QVariant NodeGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionChange && scene()) {
		//QPointF newPos = value.toPointF();
		updateLinks();
	}
	return QGraphicsItem::itemChange(change, value);
}