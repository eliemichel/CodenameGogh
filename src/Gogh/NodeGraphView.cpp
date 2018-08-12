#include "NodeGraphView.h"

#include "Logger.h"
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
#include <QMenu>
#include <QTimer>

#include <math.h>

NodeGraphView::NodeGraphView(QWidget *parent)
	: QGraphicsView(parent)
	, m_model(nullptr)
	, m_selectionModel(nullptr)
	, m_zoom(1.f)
	, m_isMovingNodes(false)
	, m_currentToolState(DefaultToolState)
	, m_panTool(this)
	, m_cutTool(this)
{
	setBackgroundBrush(QColor(57, 57, 57));
	setRenderHint(QPainter::Antialiasing);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	createActions();
}

void NodeGraphView::setModel(NodeGraphModel *model)
{
	if (m_model)
	{
		disconnect(m_model, 0, this, 0);
	}
	m_model = model;
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
	painter->drawPath(m_cutTool.cutShape());
}

void NodeGraphView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_panTool.isActive())
	{
		m_panTool.update(event->pos());
	}
	else if (m_isMovingNodes)
	{
		updateMoveNodes(event->pos());
	}
	else if (m_cutTool.isActive())
	{
		m_cutTool.update(event->pos());
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
		m_panTool.start(event->pos());
	}
	else if (event->button() == Qt::LeftButton)
	{
		switch (m_currentToolState) {
		case DefaultToolState:
		{
			QGraphicsItem *item = itemAt(event->pos());
			if (NodeGraphicsItem *nodeItem = nodeGraphScene()->toNodeItem(item))
			{
				if (!m_selectionModel)
				{
					return;
				}

				const QModelIndex & index = model()->index(nodeItem->node()->nodeIndex(), 0);
				if (!m_selectionModel->isSelected(index))
				{
					bool addToSelection = (event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier;
					m_selectionModel->select(index, addToSelection ? QItemSelectionModel::Select : QItemSelectionModel::ClearAndSelect);
				}

				startMoveNodes(event->pos());
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
					// Remove linkes once the currently queued events are processed
					QTimer::singleShot(0, this, [=]() {
						delete l;
					});
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

		case CutToolState:
		{
			m_cutTool.start(event->pos());
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
		if (m_panTool.isActive())
		{
			m_panTool.finish(event->pos());
		}
	}
	else if (event->button() == Qt::LeftButton)
	{
		if (m_cutTool.isActive())
		{
			m_cutTool.finish(event->pos());
		}
		if (m_isMovingNodes)
		{
			finishMoveNodes();
		}
	}

	QGraphicsView::mouseReleaseEvent(event);
}

void NodeGraphView::wheelEvent(QWheelEvent *event)
{
	float oldZoom = m_zoom;

	// 1.1 ^ angle makes zoom exponential and reversible
	m_zoom *= exp(event->angleDelta().y() / 180.f * 2.f * log(1.1f));

	m_zoom = std::max(0.1f, std::min(m_zoom, 2.0f));

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
		m_currentToolState = CutToolState;
	}
	else if (event->key() == Qt::Key_A && event->modifiers() & Qt::ControlModifier)
	{
		selectAll();
	}
	else if (event->key() == Qt::Key_Delete)
	{
		if (selectionModel())
		{
			while (!selectionModel()->selectedIndexes().empty())
			{
				int nodeIndex = selectionModel()->selectedIndexes().back().row();
				model()->removeRow(nodeIndex);
			}
		}
	}
	else
	{
		QGraphicsView::keyPressEvent(event);
	}
}

void NodeGraphView::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_X)
	{
		m_currentToolState = DefaultToolState;
	}
	else
	{
		QGraphicsView::keyReleaseEvent(event);
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
			for (SlotGraphicsItem *otherSlotItem : m_pendingLinksSources)
			{
				if (slotItem->isInput() != otherSlotItem->isInput())
				{
					SlotGraphicsItem *destinationSlotItem = otherSlotItem->isInput() ? otherSlotItem : slotItem;
					SlotGraphicsItem *sourceSlotItem = otherSlotItem->isInput() ? slotItem : otherSlotItem;

					// TODO: get rid of cast
					const SlotIndex & origin = sourceSlotItem->slotIndex();
					const SlotIndex & destination = destinationSlotItem->slotIndex();
					static_cast<NodeGraphModel*>(model())->addLink(origin, destination);
				}
			}

			p = item->sceneBoundingRect().center();
		}

		// TODO: is this useful ?
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

void NodeGraphView::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	for (QAction *action : m_addNodeActions)
	{
		menu.addAction(action);
	}
	m_newNodePos = mapToScene(event->pos());
	menu.exec(event->globalPos());
}

void NodeGraphView::createActions()
{
	for (QAction *action : m_addNodeActions)
	{
		delete action;
	}
	m_addNodeActions.clear();

	for (NodeType type : NodeType::availableTypes())
	{
		QAction *action = new QAction(tr("Add ") + QString::fromStdString(type.name()) + tr(" Node"), this);
		connect(action, &QAction::triggered, [=]() {
			Node *node = type.create();
			node->x = m_newNodePos.x();
			node->y = m_newNodePos.y();
			node->name = type.name();
			model()->addNode(node);
		});
		m_addNodeActions.push_back(action);
	}
}

void NodeGraphView::startMoveNodes(QPoint position)
{
	if (!selectionModel())
	{
		return;
	}

	m_nodeMoveData.clear();
	for (const QModelIndex & index : selectionModel()->selectedIndexes())
	{
		const QModelIndex & colX = model()->index(index.row(), NodeGraphModel::PosXColumn, index.parent());
		const QModelIndex & colY = model()->index(index.row(), NodeGraphModel::PosYColumn, index.parent());
		QPointF startPos = QPointF(model()->data(colX).toFloat(), model()->data(colY).toFloat());
		m_nodeMoveData.push_back(NodeMoveData(colX, colY, startPos));
		m_moveStartPos = position;
		m_isMovingNodes = true;
	}
}

void NodeGraphView::updateMoveNodes(QPoint position)
{
	QPointF delta = mapToScene(m_moveStartPos) - mapToScene(position);
	if (model())
	{
		for (const NodeMoveData & data : m_nodeMoveData)
		{
			model()->setData(data.posXIndex, data.startPos.x() - delta.x());
			model()->setData(data.posYIndex, data.startPos.y() - delta.y());
		}
	}
}

void NodeGraphView::finishMoveNodes()
{
	m_isMovingNodes = false;
}

void NodeGraphView::selectAll()
{
	if (!selectionModel() || !model())
	{
		return;
	}

	int n = model()->rowCount();
	const QModelIndex & topLeft = model()->index(0, 0);
	const QModelIndex & bottomRight = model()->index(n - 1, 0);
	selectionModel()->select(QItemSelection(topLeft, bottomRight), QItemSelectionModel::Select);
}

void NodeGraphView::onSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	if (!nodeGraphScene())
	{
		return;
	}

	for (const QModelIndex & index : deselected.indexes())
	{
		if (NodeGraphicsItem *nodeItem = nodeGraphScene()->nodeItemAtIndex(index.row()))
		{
			nodeItem->setSelected(false);
		}
	}

	for (const QModelIndex & index : selected.indexes())
	{
		if (NodeGraphicsItem *nodeItem = nodeGraphScene()->nodeItemAtIndex(index.row()))
		{
			nodeItem->setSelected(true);
		}
	}
}
