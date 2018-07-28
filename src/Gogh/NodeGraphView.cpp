#include "NodeGraphView.h"

#include "Logger.h"
#include "NodeWidget.h"
#include "LinkGraphicsItem.h"
#include "SlotGraphicsItem.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QMimeData>
#include <QDrag>

NodeGraphView::NodeGraphView(QWidget *parent)
	: QGraphicsView(parent)
	, m_zoom(1.f)
	, m_isPanning(false)
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
		QGraphicsItem *item = itemAt(event->pos());
		QVariant v = item->data(RoleData);
		if (v.isValid() && v.toInt() == SlotRole)
		{
			// If press on a slot, start dragging link

			// create pending link
			LinkGraphicsItem *link = new LinkGraphicsItem();
			link->setStartPos(item->sceneBoundingRect().center());
			link->setEndPos(mapToScene(event->pos()));
			scene()->addItem(link);
			m_pendingLinks.push_back(link);
			SlotGraphicsItem *slotItem = static_cast<SlotGraphicsItem*>(item);
			m_pendingLinksSources.push_back(slotItem);

			// create drag action
			QMimeData *mimeData = new QMimeData();
			mimeData->setData("application/x-gogh-slot", QByteArray());
			QDrag *drag = new QDrag(this);
			drag->setMimeData(mimeData);
			drag->setHotSpot(event->pos());
			drag->exec();

			for (LinkGraphicsItem *l : m_pendingLinks)
			{
				scene()->removeItem(l);
				delete l;
			}
			m_pendingLinks.clear();
			m_pendingLinksSources.clear();
		}
		else
		{
			QGraphicsView::mousePressEvent(event);
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
	else
	{
		QGraphicsView::mouseReleaseEvent(event);
	}
}

void NodeGraphView::wheelEvent(QWheelEvent *event)
{
	float oldZoom = m_zoom;

	// 1.1 ^ angle makes zoom exponential and reversible
	m_zoom *= exp(event->angleDelta().y()/180.f * 2.f * log(1.1f));

	// /!\ relative zoom has a risk of numerical error accumulation
	// this would be changed if we would go for a manual management of the view transform
	float s = m_zoom / oldZoom;
	scale(s, s);

	update();
}

void NodeGraphView::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-gogh-slot") && event->source() == this)
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void NodeGraphView::dragMoveEvent(QDragMoveEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-gogh-slot") && event->source() == this)
	{
		QGraphicsItem *item = itemAt(event->pos());
		QVariant v = item->data(RoleData);
		bool isOnSlot = v.isValid() && v.toInt() == SlotRole;
		QPointF p = isOnSlot ? item->sceneBoundingRect().center() : mapToScene(event->pos());
		
		for (LinkGraphicsItem *l : m_pendingLinks)
		{
			l->setEndPos(p);
		}

		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void NodeGraphView::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-gogh-slot") && event->source() == this)
	{
		QGraphicsItem *item = itemAt(event->pos());
		QVariant v = item->data(RoleData);
		bool isOnSlot = v.isValid() && v.toInt() == SlotRole;

		if (isOnSlot)
		{
			// Create actual links in place of temporary pending links
			SlotGraphicsItem *slotItem = static_cast<SlotGraphicsItem*>(item);
			Slot *slot = slotItem->slot();
			for (SlotGraphicsItem *sourceSlotItem : m_pendingLinksSources)
			{
				Slot *sourceSlot = sourceSlotItem->slot();
				if (slot && sourceSlot)
				{
					LinkGraphicsItem *link = new LinkGraphicsItem();
					scene()->addItem(link);
					slot->addInputLink(link);
					slot->setSourceSlot(sourceSlot);
					sourceSlot->addOutputLink(link);
					sourceSlotItem->updateLinks();
				}
			}
			slotItem->updateLinks();
		}
		QPointF p = isOnSlot ? item->sceneBoundingRect().center() : mapToScene(event->pos());

		for (LinkGraphicsItem *l : m_pendingLinks)
		{
			l->setEndPos(p);
		}

		event->accept();
	}
	else
	{
		event->ignore();
	}
}
