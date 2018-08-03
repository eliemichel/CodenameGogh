#include "NodeGraphView.h"

#include "Logger.h"
#include "NodeWidget.h"
#include "LinkGraphicsItem.h"
#include "SlotGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphModel.h"
#include "NodeGraphScene.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QMimeData>
#include <QDrag>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <math.h>

//using std::ceil;

NodeGraphView::NodeGraphView(QWidget *parent)
	: QGraphicsView(parent)
	, m_model(nullptr)
	, m_selectionModel(nullptr)
	, m_zoom(1.f)
	, m_isPanning(false)
	, m_isCutting(false)
	, m_isMovingNodes(false)
	, m_currentTool(DefaultTool)
{
	setBackgroundBrush(QColor(57, 57, 57));
	setRenderHint(QPainter::Antialiasing);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

NodeGraphView::~NodeGraphView()
{

}

void NodeGraphView::setModel(QAbstractItemModel *model)
{
	if (m_model)
	{
		disconnect(m_model, 0, this, 0);
	}
	m_model = model;
	if (m_model)
	{
		connect(m_model, &QAbstractItemModel::dataChanged, this, &NodeGraphView::onDataChanged);
	}
}

void NodeGraphView::setSelectionModel(QItemSelectionModel *selectionModel)
{
	if (m_selectionModel)
	{
		disconnect(m_selectionModel, 0, this, 0);
	}
	m_selectionModel = selectionModel;
	if (m_selectionModel)
	{
		connect(m_selectionModel, &QItemSelectionModel::currentChanged, this, &NodeGraphView::onCurrentChanged);
		connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &NodeGraphView::onSelectionChanged);
	}
}

NodeGraphScene * NodeGraphView::nodeGraphScene() const
{
	return static_cast<NodeGraphScene*>(scene());
}

void NodeGraphView::setScene(NodeGraphScene * scene)
{
	QGraphicsView::setScene(scene);
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

	// Cut shape
	painter->setBrush(Qt::NoBrush);
	painter->setPen(QPen(QColor(128, 128, 128), 1, Qt::DotLine));
	painter->drawPath(m_cutShape);
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
		if (model())
		{
			for (const NodeMoveData & data : m_nodeMoveData)
			{
				model()->setData(data.posXIndex, data.startPos.x() - delta.x());
				model()->setData(data.posYIndex, data.startPos.y() - delta.y());
			}
		}
	}
	else if (m_isCutting)
	{
		updateCut(event->pos());
	}
	else
	{
		QGraphicsView::mouseMoveEvent(event);
	}
}

void NodeGraphView::mousePressEvent(QMouseEvent *event)
{
	if (!nodeGraphScene())
	{
		return;
	}

	if (event->button() == Qt::MiddleButton)
	{
		m_isPanning = true;
		m_pressPos = event->pos();
		m_pressCenter = mapToScene(viewport()->rect().center());
	}
	else if (event->button() == Qt::LeftButton)
	{
		switch (m_currentTool) {
		case DefaultTool:
		{
			QGraphicsItem *item = itemAt(event->pos());
			if (NodeGraphicsItem *nodeItem = nodeGraphScene()->toNodeItem(item))
			{
				if (!m_selectionModel)
				{
					return;
				}

				if (!m_selectionModel->isSelected(nodeItem->modelIndex()))
				{
					bool addToSelection = (event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier;
					m_selectionModel->select(nodeItem->modelIndex(), addToSelection ? QItemSelectionModel::Select : QItemSelectionModel::ClearAndSelect);
				}

				m_nodeMoveData.clear();
				for (const QModelIndex & index : m_selectionModel->selectedIndexes())
				{
					const QModelIndex & colX = model()->index(index.row(), NodeGraphModel::PosXColumn, index.parent());
					const QModelIndex & colY = model()->index(index.row(), NodeGraphModel::PosYColumn, index.parent());
					QPointF startPos = QPointF(model()->data(colX).toFloat(), model()->data(colY).toFloat());
					m_nodeMoveData.push_back(NodeMoveData(colX, colY, startPos));
					m_moveStartPos = event->pos();
					m_isMovingNodes = true;
				}
			}
			else if (SlotGraphicsItem *slotItem = nodeGraphScene()->toSlotItem(item))
			{
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
				m_selectionModel->select(QItemSelection(), QItemSelectionModel::Clear);
				QGraphicsView::mousePressEvent(event);
			}
			break;
		}

		case CutTool:
		{
			startCut(event->pos());
			break;
		}
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
		if (m_isCutting)
		{
			updateCut(event->pos());
			finishCut();
		}
		m_isMovingNodes = false;
		m_isCutting = false;
	}

	QGraphicsView::mouseReleaseEvent(event);
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

void NodeGraphView::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_X)
	{
		m_currentTool = CutTool;
	}
}

void NodeGraphView::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_X)
	{
		m_currentTool = DefaultTool;
	}
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
	if (!nodeGraphScene())
	{
		return;
	}

	if (event->mimeData()->hasFormat("application/x-gogh-slot") && event->source() == this)
	{
		QGraphicsItem *item = itemAt(event->pos());
		bool isOnSlot = nodeGraphScene()->toSlotItem(item) != nullptr;
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
	if (!nodeGraphScene())
	{
		return;
	}

	if (event->mimeData()->hasFormat("application/x-gogh-slot") && event->source() == this)
	{
		QGraphicsItem *item = itemAt(event->pos());
		QPointF p = mapToScene(event->pos());
		if (SlotGraphicsItem *slotItem = nodeGraphScene()->toSlotItem(item))
		{
			// Create actual links in place of temporary pending links if possible
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
					link->setEndSlotItem(destinationSlotItem);
					scene()->addItem(link);
					destinationSlotItem->setInputLink(link);
					sourceSlotItem->addOutputLink(link);

					destinationSlotItem->updateLinks();
					sourceSlotItem->updateLinks();

					destinationSlot->setSourceSlot(sourceSlot);
				}
			}

			p = item->sceneBoundingRect().center();
		}

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

void NodeGraphView::startCut(QPoint position)
{
	m_isCutting = true;
	m_cutShape = QPainterPath();
	m_cutShape.moveTo(mapToScene(position));
}

void NodeGraphView::updateCut(QPoint position)
{
	QPointF oldPos = m_cutShape.currentPosition();
	QPointF curPos = mapToScene(position);
	m_cutShape.lineTo(curPos);
	QPointF topLeft(std::min(curPos.x(), oldPos.x()), std::min(curPos.y(), oldPos.y()));
	QSizeF size(std::abs(curPos.x() - oldPos.x()), std::abs(curPos.y() - oldPos.y()));
	scene()->update(QRectF(topLeft, size));

	// test collision
	QPainterPath edge;
	edge.moveTo(oldPos);
	edge.lineTo(curPos);
	QList<QGraphicsItem*> items = scene()->items(edge);
	for (QGraphicsItem *item : items)
	{
		if (LinkGraphicsItem *linkItem = nodeGraphScene()->toLinkItem(item))
		{
			if (SlotGraphicsItem *slotItem = linkItem->endSlotItem())
			{
				DEBUG_LOG << "remove link";
				slotItem->setInputLink(nullptr);
			}
			else
			{
				scene()->removeItem(item);
				delete item;
			}
		}
	}
}

void NodeGraphView::finishCut()
{
	m_cutShape = QPainterPath();
}

void NodeGraphView::onDataChanged()
{
}

void NodeGraphView::onCurrentChanged()
{
}

void NodeGraphView::onSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	if (!nodeGraphScene())
	{
		return;
	}

	for (const QModelIndex & index : deselected.indexes())
	{
		if (NodeGraphicsItem *nodeItem = nodeGraphScene()->nodeItemAtIndex(index))
		{
			nodeItem->setSelected(false);
		}
	}

	for (const QModelIndex & index : selected.indexes())
	{
		if (NodeGraphicsItem *nodeItem = nodeGraphScene()->nodeItemAtIndex(index))
		{
			nodeItem->setSelected(true);
		}
	}
}
