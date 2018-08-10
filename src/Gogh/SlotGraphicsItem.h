#ifndef H_SLOTGRAPHICSITEM
#define H_SLOTGRAPHICSITEM

#include <QGraphicsEllipseItem>

#include "Logger.h"

class Slot;
class QGraphicsSceneMouseEvent;
class LinkGraphicsItem;
class NodeGraphicsItem;

class SlotGraphicsItem : public QGraphicsEllipseItem
{
public:
	SlotGraphicsItem(QGraphicsItem *parent = nullptr);

	Slot *slot() const { return m_slot; }
	void setSlot(Slot *slot);

	LinkGraphicsItem * inputLink() const { return m_inputLink; }
	void setInputLink(LinkGraphicsItem *link);

private:
	Slot *m_slot;
	LinkGraphicsItem *m_inputLink;
};

#endif // H_SLOTGRAPHICSITEM
