#include "NodeGraphicsItem.h"
#include "LinkGraphicsItem.h"
#include "NodeWidget.h"
#include "Logger.h"
#include "NodeGraphView.h"
#include "SlotGraphicsItem.h"
#include "NodeGraphModel.h"
#include "NodeGraphScene.h"

#include <QWidget>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>
#include <QPainter>

void NodeGraphicsItemControl::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	qreal round = 5;

	QRectF r = rect();
	painter->setPen(pen());
	painter->setBrush(brush());
	painter->drawRoundedRect(r.x(), r.y(), r.width(), r.height(), round, round);

	painter->setPen(Qt::NoPen);
	painter->drawRect(r.x(), r.y() + round, r.width(), r.height() - round);

	painter->setPen(pen());
	painter->setBrush(Qt::NoBrush);
	painter->drawLine(r.x(), r.y() + round, r.x(), r.y() + r.height());
	painter->drawLine(r.x(), r.y() + r.height(), r.x() + r.width(), r.y() + r.height());
	painter->drawLine(r.x() + r.width(), r.y() + r.height(), r.x() + r.width(), r.y() + round);
	
	painter->setPen(pen() == Qt::NoPen ? QPen(Qt::white) : pen());
	painter->drawText(r.x() + 5, r.y() + 2, r.width() - 10, r.height() - 4, Qt::AlignLeft || Qt::AlignVCenter, name());
}

NodeGraphicsItem::NodeGraphicsItem(QGraphicsScene *scene, NodeWidget *content)
	: m_content(content)
{
	content->resize(content->sizeHint());

	m_control = new NodeGraphicsItemControl();
	scene->addItem(m_control);
	m_control->setRect(0, 0, content->width(), 20);
	m_control->setPen(Qt::NoPen);
	m_control->setBrush(QBrush(QColor(41, 41, 41)));
	m_control->setFlag(QGraphicsItem::ItemIsSelectable, true);
	m_control->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

	// data
	m_control->setData(NodeGraphScene::RoleData, NodeGraphScene::NodeControlRole);
	QVariant v;
	v.setValue<NodeGraphicsItem*>(this);
	m_control->setData(NodeGraphScene::NodePointerData, v);
	m_control->setZValue(NodeGraphScene::NodeLayer);

	m_proxy = scene->addWidget(content);
	m_proxy->setData(NodeGraphScene::RoleData, NodeGraphScene::NodeContentRole);
	m_proxy->setPos(0, m_control->rect().height());
	m_proxy->setParentItem(m_control);
	m_proxy->setZValue(NodeGraphScene::NodeLayer);

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

void NodeGraphicsItem::setSelected(bool selected)
{
	m_isSelected = selected;
	//m_control->setBrush(QBrush(m_isSelected ? QColor(64, 64, 64) : QColor(41, 41, 41)));
	m_control->setPen(m_isSelected ? QPen(QColor(255, 128, 0)) : Qt::NoPen);
}

void NodeGraphicsItem::updateLinks() const
{
	for (SlotGraphicsItem *item : m_slotItems)
	{
		item->updateLinks();
	}
}

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
		QString name;
		const QAbstractItemModel *model = modelIndex().model();
		x = model->data(model->index(modelIndex().row(), NodeGraphModel::PosXColumn, modelIndex().parent())).toInt();
		y = model->data(model->index(modelIndex().row(), NodeGraphModel::PosYColumn, modelIndex().parent())).toInt();
		name = model->data(model->index(modelIndex().row(), NodeGraphModel::NameColumn, modelIndex().parent())).toString();
		setPos(QPointF(x, y));
		m_control->setName(name);
		updateLinks();
	}
}
