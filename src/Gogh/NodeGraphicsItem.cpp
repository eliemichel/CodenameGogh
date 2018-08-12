#include "NodeGraphicsItem.h"
#include "LinkGraphicsItem.h"
#include "Logger.h"
#include "NodeGraphView.h"
#include "SlotGraphicsItem.h"
#include "NodeGraphModel.h"
#include "NodeGraphScene.h"
#include "Node.h"

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

NodeGraphicsItem::NodeGraphicsItem(NodeGraphScene *scene, Node *node)
	: m_node(node)
	, m_graphScene(scene)
	, m_control(nullptr)
{
	m_content = node->createEditor();
	m_content->resize(m_content->sizeHint());

	m_control = new NodeGraphicsItemControl();
	scene->addItem(m_control);
	m_control->setRect(0, 0, m_content->width(), 20);
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

	m_proxy = scene->addWidget(m_content);
	m_proxy->setData(NodeGraphScene::RoleData, NodeGraphScene::NodeContentRole);
	m_proxy->setPos(0, m_control->rect().height());
	m_proxy->setParentItem(m_control);
	m_proxy->setZValue(NodeGraphScene::NodeLayer);

	update();
}

NodeGraphicsItem::~NodeGraphicsItem()
{
	for (SlotGraphicsItem *inputItem : m_inputSlotItems)
	{
		inputItem->removeInputLink();
	}
	m_graphScene->removeItem(m_control);
	delete m_control;
}

void NodeGraphicsItem::setSelected(bool selected)
{
	m_isSelected = selected;
	m_control->setPen(m_isSelected ? QPen(QColor(255, 128, 0)) : Qt::NoPen);
}

void NodeGraphicsItem::updateInputSlots()
{
	for (int i = 0 ; i < node()->inputSlotCount() ; ++i)
	{
		SlotGraphicsItem *slotItem;
		if (i >= m_inputSlotItems.size())
		{
			slotItem = new SlotGraphicsItem();
			m_graphScene->addItem(slotItem);
			m_inputSlotItems.push_back(slotItem);
		}
		else
		{
			slotItem = m_inputSlotItems[i];
		}

		slotItem->setSlotIndex(SlotIndex(node()->nodeIndex(), i));
		slotItem->setIsInput(true);
		slotItem->setPos(-7, 30 + i * 30);
		slotItem->setParentItem(m_control);
	}
}

void NodeGraphicsItem::updateOutputSlots()
{
	for (int i = 0 ; i < node()->outputSlotCount(); ++i)
	{
		SlotGraphicsItem *slotItem;
		if (i >= m_outputSlotItems.size())
		{
			slotItem = new SlotGraphicsItem();
			m_graphScene->addItem(slotItem);
			m_outputSlotItems.push_back(slotItem);
		}
		else
		{
			slotItem = m_outputSlotItems[i];
		}

		slotItem->setSlotIndex(SlotIndex(node()->nodeIndex(), i));
		slotItem->setIsInput(false);
		slotItem->setPos(m_control->rect().width() - 8, 30 + i * 30);
		slotItem->setParentItem(m_control);
	}
}

void NodeGraphicsItem::updateInputLinks() const
{
	int slotId = -1;
	for (SlotGraphicsItem *item : m_inputSlotItems)
	{
		++slotId;

		if (!node())
		{
			item->removeInputLink();
			continue;
		}

		// TODO: use node()->originSlot when it will exist
		SlotIndex sourceSlot = node()->inputLinks[slotId];

		if (!sourceSlot.isValid())
		{
			item->removeInputLink();
			continue;
		}

		NodeGraphicsItem *sourceNodeItem = m_graphScene->nodeItemAtIndex(sourceSlot.node);
		QPointF startPos = sourceNodeItem->outputSlotPosition(sourceSlot.slot);

		item->ensureInputLink();
		item->inputLink()->setStartPos(startPos);
		item->inputLink()->setEndPos(item->slotCenter());
		item->inputLink()->update();
	}
}

void NodeGraphicsItem::updateOutputLinks() const
{
	int slotId = -1;
	for (SlotGraphicsItem *item : m_outputSlotItems)
	{
		++slotId;

		if (!node())
		{
			continue;
		}

		// TODO: use node()->destinationSlots when it will exist
		const std::set<SlotIndex> & destinationSlots = node()->outputLinks[slotId];
		for (const SlotIndex & destination : destinationSlots)
		{
			if (NodeGraphicsItem *destinationNodeItem = m_graphScene->nodeItemAtIndex(destination.node))
			{
				destinationNodeItem->updateInputLinks();
			}
		}
	}
}

QPointF NodeGraphicsItem::outputSlotPosition(int i) const
{
	if (i < 0 || i >= m_outputSlotItems.size())
	{
		return QPointF();
	}
	else
	{
		return m_outputSlotItems[i]->sceneBoundingRect().center();
	}
}

void NodeGraphicsItem::update()
{
	setPos(QPointF(node()->x, node()->y));
	m_control->setName(QString::fromStdString(node()->name));

	updateInputSlots();
	updateOutputSlots();

	updateInputLinks();
	updateOutputLinks();
}
