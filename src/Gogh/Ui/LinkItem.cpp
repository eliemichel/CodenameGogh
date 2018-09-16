#include "LinkItem.h"
#include "Link.h"
#include "QuadTree.h"
#include "SlotItem.h"

LinkItem::LinkItem(OutputSlotItem *originItem, InputSlotItem *destinationItem, Link *link, QuadTree *tree)
	: AbstractNodeAreaItem({ 0, 0, 200, 100 }, LinkItemType)
	, m_originItem(originItem)
	, m_destinationItem(destinationItem)
	, m_link(link)
{
	if (tree) {
		tree->Insert(this);
	}

	link->destroyed.connect(this, &LinkItem::OnLinkDestroyed);
}

void LinkItem::Paint(NVGcontext *vg) const {
	if (!m_link) {
		return;
	}

	const ::Rect & o = m_originItem->BBox();
	const ::Rect & d = m_destinationItem->BBox();

	float ox = o.xf() + o.wf() / 2.f;
	float oy = o.yf() + o.hf() / 2.f;
	float dx = d.xf() + d.wf() / 2.f;
	float dy = d.yf() + d.hf() / 2.f;

	nvgBeginPath(vg);
	nvgMoveTo(vg, ox, oy);
	nvgLineTo(vg, dx, dy);
	nvgStrokeColor(vg, nvgRGB(255, 255, 255));
	nvgStroke(vg);
}

void LinkItem::OnLinkDestroyed() {
	m_link = nullptr;
}
