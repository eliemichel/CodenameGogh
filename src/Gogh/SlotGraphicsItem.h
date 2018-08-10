#ifndef H_SLOTGRAPHICSITEM
#define H_SLOTGRAPHICSITEM

#include <QGraphicsEllipseItem>

#include "Logger.h"
#include "SlotIndex.h"

class Slot;
class QGraphicsSceneMouseEvent;
class LinkGraphicsItem;
class NodeGraphicsItem;

class SlotGraphicsItem : public QGraphicsEllipseItem
{
public:
	SlotGraphicsItem(QGraphicsItem *parent = nullptr);

	Slot *slot() const { return m_slot; }

	const SlotIndex & slotIndex() const { return m_slotIndex; }
	void setSlotIndex(const SlotIndex & slotIndex) { m_slotIndex = slotIndex; }

	bool isInput() const { return m_isInput; }
	void setIsInput(bool isInput) { m_isInput = isInput; }

	LinkGraphicsItem * inputLink() const { return m_inputLink; }
	void setInputLink(LinkGraphicsItem *link);

private:
	Slot *m_slot;
	SlotIndex m_slotIndex;
	bool m_isInput;
	LinkGraphicsItem *m_inputLink;
};

#endif // H_SLOTGRAPHICSITEM
