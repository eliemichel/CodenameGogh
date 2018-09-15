#ifndef H_SLOTITEM
#define H_SLOTITEM

#include "NodeAreaItems.h"

class InputSlot;
class OutputSlot;

class AbstractSlotItem : public AbstractNodeAreaItem {
public:
	AbstractSlotItem(Rect bbox, int type)
		: AbstractNodeAreaItem(bbox, type)
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

class InputSlotItem : public AbstractSlotItem {
public:
	static InputSlotItem * fromRawItem(QuadTree::Item *rawItem) {
		if (rawItem && rawItem->Type() == InputSlotItemType) {
			return static_cast<InputSlotItem*>(rawItem);
		} else {
			return nullptr;
		}
	}

public:
	InputSlotItem(InputSlot *slot, Rect bbox)
		: AbstractSlotItem(bbox, InputSlotItemType)
		, m_slot(slot)
	{}

	InputSlot * Slot() const { return m_slot; }

private:
	InputSlot *m_slot;
};

class OutputSlotItem : public AbstractSlotItem {
public:
	static OutputSlotItem * fromRawItem(QuadTree::Item *rawItem) {
		if (rawItem && rawItem->Type() == OutputSlotItemType) {
			return static_cast<OutputSlotItem*>(rawItem);
		} else {
			return nullptr;
		}
	}

public:
	OutputSlotItem(OutputSlot *slot, Rect bbox)
		: AbstractSlotItem(bbox, OutputSlotItemType)
		, m_slot(slot)
	{}

	OutputSlot * Slot() const { return m_slot; }

private:
	OutputSlot * m_slot;
};


#endif // H_SLOTITEM
