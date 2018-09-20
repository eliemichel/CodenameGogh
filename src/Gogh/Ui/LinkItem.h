#ifndef H_LINKITEM
#define H_LINKITEM

#include "NodeAreaItems.h"

class Link;
class InputSlotItem;
class OutputSlotItem;
class QuadTree;

class LinkItem : public AbstractNodeAreaItem {
public:
	/// origin and destination items must match the link object. They are used for positionning the links.
	LinkItem(OutputSlotItem *originItem, InputSlotItem *destinationItem, ::Link *link, QuadTree *tree = nullptr);
	~LinkItem();

	::Link * Link() const { return m_link; }

	void Paint(NVGcontext *vg) const override;

private:
	void UpdateRect();
	void OnLinkDestroyed();

private:
	::Link *m_link;
	OutputSlotItem *m_originItem;
	InputSlotItem *m_destinationItem;
	SignalConnection m_originMovedConnection;
	SignalConnection m_destinationMovedConnection;
};

#endif // H_LINKITEM