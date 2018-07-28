#include "NodeGraphView.h"

#include "Logger.h"
#include "NodeWidget.h"
#include "LinkGraphicsItem.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>

NodeGraphView::NodeGraphView(QWidget *parent)
	: QGraphicsView(parent)
	, m_zoom(1.f)
	, m_isPanning(false)
	, m_isMovingNode(false)
	, m_movingItem(nullptr)
{
	setBackgroundBrush(QColor(64, 64, 64));
	setRenderHint(QPainter::Antialiasing);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

NodeGraphView::~NodeGraphView()
{

}

void NodeGraphView::drawBackground(QPainter *painter, const QRectF &rect)
{
	painter->setBrush(backgroundBrush());
	painter->drawRect(rect);
	
	QPen pen(QColor(80, 80, 80), 1, Qt::DotLine);
	// thicker pen for one line out of 5
	QPen steppen(QColor(80, 80, 80));

	painter->setPen(pen);
	
	float step = 10.f;

	// number of columns
	int n = static_cast<int>(ceil(rect.width() / step));
	// number of lines
	int m = static_cast<int>(ceil(rect.height() / step));

	int offsetXint = static_cast<int>(floor(rect.left() / step));
	int offsetYint = static_cast<int>(floor(rect.top() / step));

	// Draw columns
	for (int i = 0; i < n; ++i)
	{
		if ((i + offsetXint) % 5 == 0)
		{
			painter->setPen(steppen);
		}
		float x = (i + offsetXint) * step;
		painter->drawLine(x, rect.bottom(), x, rect.top());
		painter->setPen(pen);
	}
	
	// Draw lines
	for (int j = 0; j < m; ++j)
	{
		if ((j + offsetYint) % 5 == 0)
		{
			painter->setPen(steppen);
		}
		float y = (j + offsetYint) * step;
		painter->drawLine(rect.left(), y, rect.right(), y);
		painter->setPen(pen);
	}
}

void NodeGraphView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_isPanning)
	{
		QPointF c = mapToScene(viewport()->rect().center());
		QPointF delta = mapToScene(m_pressPos) - mapToScene(event->pos());
		centerOn(m_pressCenter + delta);
	}
	else if (m_isMovingNode && m_movingItem)
	{
		QPointF c = mapToScene(viewport()->rect().center());
		QPointF delta = mapToScene(m_pressPos) - mapToScene(event->pos());
		m_movingItem->setPos(m_movingItemStartPos - delta);

		// update link
		QVariant itemType = m_movingItem->data(0);
		if (itemType.isValid() && itemType.toInt() == 1) // if is node
		{
			QGraphicsProxyWidget *proxy = static_cast<QGraphicsProxyWidget*>(m_movingItem->toGraphicsObject());
			NodeWidget *node = static_cast<NodeWidget*>(proxy->widget());
			for (Slot *s : node->allSlots())
			{
				for (LinkGraphicsItem *l : s->inputLinks())
				{
					l->setEndPos(s->pos() + proxy->scenePos());
				}
				for (LinkGraphicsItem *l : s->outputLinks())
				{
					l->setStartPos(s->pos() + proxy->scenePos());
				}
			}
		}
	}
	else
	{
		QGraphicsView::mouseMoveEvent(event);
	}
}

void NodeGraphView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MiddleButton)
	{
		m_isPanning = true;
		m_pressPos = event->pos();
		m_pressCenter = mapToScene(viewport()->rect().center());
	}
	else if (event->button() == Qt::LeftButton)
	{
		if (QGraphicsItem *item = itemAt(event->pos()))
		{
			m_isMovingNode = true;
			m_movingItem = item;
			m_pressPos = event->pos();
			m_movingItemStartPos = m_movingItem->pos();
		}
	}
	else
	{
		QGraphicsView::mousePressEvent(event);
	}
}

void NodeGraphView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MiddleButton)
	{
		m_isPanning = false;
	}
	else if (event->button() == Qt::LeftButton)
	{
		m_isMovingNode = false;
	}
	else
	{
		QGraphicsView::mouseReleaseEvent(event);
	}
}

void NodeGraphView::wheelEvent(QWheelEvent *event)
{
	// TODO: when zooming, we should pause and restart the ongoing panning/moving actions

	float oldZoom = m_zoom;

	// 1.1 ^ angle makes zoom exponential and reversible
	m_zoom *= exp(event->angleDelta().y()/180.f * log(1.1f));

	// /!\ relative zoom has a risk of numerical error accumulation
	// this would be changed if we would go for a manual management of the view transform
	float s = m_zoom / oldZoom;
	scale(s, s);

	update();
}
