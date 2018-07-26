#include "NodeGraphView.h"

#include "Logger.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>

NodeGraphView::NodeGraphView(QWidget *parent)
	: QGraphicsView(parent)
	, m_zoom(1.f)
	, m_isPanning(false)
{
	setBackgroundBrush(QColor(64, 64, 64));
	// 
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

	//painter.drawEllipse(m_offset, 5, 5);
	painter->setPen(QPen(QColor(255, 80, 80)));
	QPointF offset = QPointF(0.f, 0.f);
	painter->drawLine(offset - QPoint(10, 10), offset + QPoint(10, 10));
	painter->drawLine(offset - QPoint(10, -10), offset + QPoint(10, -10));
}

void NodeGraphView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_isPanning)
	{
		QPointF c = mapToScene(viewport()->rect().center());
		QPointF delta = mapToScene(m_pressPos) - mapToScene(event->pos());
		centerOn(m_pressCenter + delta);
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
	else
	{
		QGraphicsView::mouseReleaseEvent(event);
	}
}

void NodeGraphView::wheelEvent(QWheelEvent *event)
{
	// 1.1 ^ angle makes zoom exponential and reversible
	m_zoom *= exp(event->angleDelta().y()/180.f * log(1.1f));
	update();
}
