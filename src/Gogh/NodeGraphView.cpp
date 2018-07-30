#include "NodeGraphView.h"

#include "Logger.h"
#include "NodeWidget.h"
#include "LinkGraphicsItem.h"
#include "SlotGraphicsItem.h"
#include "NodeGraphicsItem.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QMimeData>
#include <QDrag>
#include <math.h>

//using std::ceil;

NodeGraphView::NodeGraphView(QWidget *parent)
	: QGraphicsView(parent)
	, m_zoom(1.f)
	, m_isPanning(false)
	, m_isMovingNodes(false)
{
	setBackgroundBrush(QColor(57, 57, 57));
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

	QPen pen(QColor(47, 47, 47), 1, Qt::DotLine);
	// thicker pen for one line out of 5
	QPen steppen(QColor(41, 41, 41));

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
	else if (m_isMovingNodes)
	{
		QPointF delta = mapToScene(m_moveStartPos) - mapToScene(event->pos());
		for (SelectionItem sel : m_selectionModel)
		{
			sel.nodeItem->setPos(sel.startPos - delta);
			sel.nodeItem->updateLinks();
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
		QGraphicsItem *item = itemAt(event->pos());
		QVariant v = item->data(RoleData);
		if (v.isValid() && v.toInt() == NodeControlRole)
		{
			NodeGraphicsItem *nodeItem = static_cast<NodeGraphicsItem*>(item);
			// If press on a slot, start moving nodes

			m_selectionModel.clear();
			SelectionItem sel(nodeItem, nodeItem->pos());
			m_selectionModel.push_back(sel);
			m_moveStartPos = event->pos();
			m_isMovingNodes = true;
		}
		else if (v.isValid() && v.toInt() == SlotRole)
		{
			SlotGraphicsItem *slotItem = static_cast<SlotGraphicsItem*>(item);
			// If press on a slot, start dragging link

			// create pending link
			LinkGraphicsItem *link = new LinkGraphicsItem();
			link->setStartPos(item->sceneBoundingRect().center());
			link->setEndPos(mapToScene(event->pos()));
			scene()->addItem(link);
			m_pendingLinks.push_back(link);
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
		if (event->button() == Qt::LeftButton)
		{
			m_isMovingNodes = false;
		}
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
			// Create actual links in place of temporary pending links if possible
			SlotGraphicsItem *slotItem = static_cast<SlotGraphicsItem*>(item);
			Slot *slot = slotItem->slot();
			for (SlotGraphicsItem *otherSlotItem : m_pendingLinksSources)
			{
				Slot *otherSlot = otherSlotItem->slot();
				if (slot && otherSlot && slot->isInput() != otherSlot->isInput())
				{
					Slot *destinationSlot = otherSlot->isInput() ? otherSlot : slot;
					Slot *sourceSlot = otherSlot->isInput() ? slot : otherSlot;
					SlotGraphicsItem *destinationSlotItem = otherSlot->isInput() ? otherSlotItem : slotItem;
					SlotGraphicsItem *sourceSlotItem = otherSlot->isInput() ? slotItem : otherSlotItem;

					LinkGraphicsItem *link = new LinkGraphicsItem();
					scene()->addItem(link);
					destinationSlotItem->setInputLink(link);
					sourceSlotItem->addOutputLink(link);

					destinationSlotItem->updateLinks();
					sourceSlotItem->updateLinks();

					destinationSlot->setSourceSlot(sourceSlot);
				}
			}
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
