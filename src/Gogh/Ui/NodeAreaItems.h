#ifndef H_NODEAREAITEMS
#define  H_NODEAREAITEMS

#include "QuadTree.h"
#include "UiBase.h"

enum NodeAreaItemType {
	_BeginNodeAreaItems,
	NodeItemType,
	SlotItemType,
	_EndNodeAreaItems,
};

/**
* Add parent/child mechanism to QuadTree::Item
*/
class AbstractNodeAreaItem : public QuadTree::Item {
public:
	static AbstractNodeAreaItem * fromRawItem(QuadTree::Item *rawItem) {
		if (rawItem
			&& rawItem->Type() > _BeginNodeAreaItems
			&& rawItem->Type() < _EndNodeAreaItems) {
			return static_cast<AbstractNodeAreaItem*>(rawItem);
		}
		else {
			return nullptr;
		}
	}

public:
	AbstractNodeAreaItem(Rect bbox, int type)
		: QuadTree::Item(bbox, type)
	{}

	void AddChild(AbstractNodeAreaItem *child) { m_children.push_back(child); }
	const std::vector<AbstractNodeAreaItem*> & Children() const { return m_children; }

	virtual void Paint(NVGcontext *vg) const {
		for (const AbstractNodeAreaItem* child : Children()) {
			child->Paint(vg);
		}
	}

private:
	std::vector<AbstractNodeAreaItem*> m_children;
};

class SlotItem : public AbstractNodeAreaItem {
public:
	static SlotItem * fromRawItem(QuadTree::Item *rawItem) {
		if (rawItem && rawItem->Type() == SlotItemType) {
			return static_cast<SlotItem*>(rawItem);
		} else {
			return nullptr;
		}
	}

public:
	SlotItem(Rect bbox)
		: AbstractNodeAreaItem(bbox, SlotItemType)
	{}

	bool Hit(float x, float y) override {
		// treated as an ellipse
		const Rect & r = BBox();
		float hw = r.wf() / 2.f;
		float hh = r.hf() / 2.f;
		float cx = r.xf() + hw;
		float cy = r.yf() + hh;
		float dx = (x - cx) / hw;
		float dy = (y - cy) / hh;
		return (dx * dx + dy * dy) < 1.f;
	}

	void Paint(NVGcontext *vg) const override {
		const Rect & r = BBox();
		float hw = r.wf() / 2.f;
		float hh = r.hf() / 2.f;
		float cx = r.xf() + hw;
		float cy = r.yf() + hh;

		nvgBeginPath(vg);
		nvgEllipse(vg, cx, cy, hw, hh);
		nvgFillColor(vg, nvgRGB(255, 255, 255));
		nvgFill(vg);

		AbstractNodeAreaItem::Paint(vg);
	}
};

#endif // H_NODEAREAITEMS
