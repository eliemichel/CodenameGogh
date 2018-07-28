#ifndef H_SLOTGRAPHICSITEM
#define H_SLOTGRAPHICSITEM

#include <QGraphicsEllipseItem>

#include "Logger.h"

class Slot;
class QGraphicsSceneMouseEvent;
class LinkGraphicsItem;

class SlotGraphicsItem : public QGraphicsEllipseItem
{
public:
	SlotGraphicsItem(QGraphicsItem *parent = nullptr);

	Slot *slot() const { return m_slot; }
	void setSlot(Slot *slot) { m_slot = slot; }

	void updateLinks() const;

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
	Slot *m_slot;
};

#endif // H_SLOTGRAPHICSITEM
