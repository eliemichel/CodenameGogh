#include "NodeGraphicsItem.h"
#include "LinkGraphicsItem.h"
#include "NodeWidget.h"
#include "Logger.h"
#include "NodeGraphView.h"
#include "SlotGraphicsItem.h"
#include "NodeGraphModel.h"

#include <QWidget>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>

NodeGraphicsItem::NodeGraphicsItem(QGraphicsScene *scene, NodeWidget *content)
	: m_content(content)
{
	content->resize(content->sizeHint());

	m_control = new QGraphicsRectItem();
	scene->addItem(m_control);
	m_control->setRect(0, 0, content->width(), 20);
	m_control->setPen(Qt::NoPen);
	m_control->setBrush(QBrush(QColor(41, 41, 41)));
	//setFlag(ItemIsMovable, true);
	m_control->setFlag(QGraphicsItem::ItemIsSelectable, true);
	m_control->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

	// data
	m_control->setData(NodeGraphView::RoleData, NodeGraphView::NodeControlRole);
	QVariant v;
	v.setValue<NodeGraphicsItem*>(this);
	m_control->setData(NodeGraphView::NodePointerData, v);

	m_proxy = scene->addWidget(content);
	m_proxy->setData(NodeGraphView::RoleData, NodeGraphView::NodeContentRole);
	m_proxy->setPos(0, m_control->rect().height());
	m_proxy->setParentItem(m_control);

	// Wrap a SlotGraphiscItem around all the slots of the content node
	int offset = 30;
	for (Slot *s : content->inputSlots())
	{
		SlotGraphicsItem *slotItem = new SlotGraphicsItem();
		scene->addItem(slotItem);
		slotItem->setSlot(s);
		slotItem->setPos(-7, offset);
		slotItem->setParentItem(m_control);
		m_slotItems.push_back(slotItem);
		offset += 30;
	}
	offset = 30;
	for (Slot *s : content->outputSlots())
	{
		SlotGraphicsItem *slotItem = new SlotGraphicsItem();
		scene->addItem(slotItem);
		slotItem->setSlot(s);
		slotItem->setPos(m_control->rect().width() - 8, offset);
		slotItem->setParentItem(m_control);
		m_slotItems.push_back(slotItem);
		offset += 30;
	}
}

void NodeGraphicsItem::setModelIndex(const QModelIndex & modelIndex)
{
	if (m_modelIndex.model())
	{
		disconnect(m_modelIndex.model(), 0, this, 0);
	}
	m_modelIndex = modelIndex;
	if (m_modelIndex.model())
	{
		connect(m_modelIndex.model(), &QAbstractItemModel::dataChanged, this, &NodeGraphicsItem::onDataChanged);
	}
}

void NodeGraphicsItem::updateLinks() const
{
	for (SlotGraphicsItem *item : m_slotItems)
	{
		item->updateLinks();
	}
}
/*
QVariant NodeGraphicsItem::itemChange(QGraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionChange && scene()) {
		updateLinks();
	}
	return QGraphicsItem::itemChange(change, value);
}
*/
void NodeGraphicsItem::onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
	const QModelIndex & currentIndex = modelIndex();
	if (topLeft.parent() == currentIndex.parent()
		&& bottomRight.parent() == currentIndex.parent()
		&& currentIndex.row() >= topLeft.row()
		&& currentIndex.row() <= bottomRight.row())
	{
		float x;
		float y;
		const QAbstractItemModel *model = modelIndex().model();
		x = model->data(model->index(modelIndex().row(), NodeGraphModel::PosXColumn, modelIndex().parent())).toInt();
		y = model->data(model->index(modelIndex().row(), NodeGraphModel::PosYColumn, modelIndex().parent())).toInt();
		setPos(QPointF(x, y));
		updateLinks();
	}
}
