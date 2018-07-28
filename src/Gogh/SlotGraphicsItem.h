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
	void setSlot(Slot *slot);

	LinkGraphicsItem * inputLink() const { return m_inputLink; }
	void setInputLink(LinkGraphicsItem *link);

	const std::vector<LinkGraphicsItem*> outputLinks() const { return m_outputLinks; }
	void addOutputLink(LinkGraphicsItem *link) { m_outputLinks.push_back(link); }
	void removeOutputLink(LinkGraphicsItem *link);

	void updateLinks() const;

private:
	Slot *m_slot;
	LinkGraphicsItem *m_inputLink;
	std::vector<LinkGraphicsItem*> m_outputLinks;
};

#endif // H_SLOTGRAPHICSITEM
