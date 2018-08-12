#ifndef H_SLOTGRAPHICSITEM
#define H_SLOTGRAPHICSITEM

#include <QGraphicsEllipseItem>

#include "Logger.h"
#include "SlotIndex.h"

class QGraphicsSceneMouseEvent;
class LinkGraphicsItem;
class NodeGraphicsItem;

class SlotGraphicsItem : public QGraphicsEllipseItem
{
public:
	SlotGraphicsItem(QGraphicsItem *parent = nullptr);
	~SlotGraphicsItem();

	/// Point to which attaching links
	QPointF slotCenter() const { return sceneBoundingRect().center(); };

	const SlotIndex & slotIndex() const { return m_slotIndex; }
	void setSlotIndex(const SlotIndex & slotIndex) { m_slotIndex = slotIndex; }

	bool isInput() const { return m_isInput; }
	void setIsInput(bool isInput) { m_isInput = isInput; }

	LinkGraphicsItem * inputLink() const { return m_inputLink; }
	void removeInputLink();
	void ensureInputLink();

private:
	SlotIndex m_slotIndex;
	bool m_isInput;
	LinkGraphicsItem *m_inputLink;
};

#endif // H_SLOTGRAPHICSITEM
