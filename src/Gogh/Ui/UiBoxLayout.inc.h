/**
* Paint Portable
* Copyright (c) 2018 - Elie Michel
*/

/*
 * Do not include this if you simply use the BaseUi toolkit.
 * This is a hacky way of defining both VBox and HBox layouts with not too much code duplication.
 * Define BUI_HBOX_IMPLEMENTATION to define the HBoxLayout and undef it for VBoxLayout
 */

#undef Ui_BoxLayout
#ifdef BUI_HBOX_IMPLEMENTATION
#define Ui_BoxLayout UiHBoxLayout
#else
#define Ui_BoxLayout UiVBoxLayout
#endif

class Ui_BoxLayout : public UiLayout {
public:
	Ui_BoxLayout()
		: UiLayout()
		, m_overflowBehavior(ShrinkOverflow)
	{}

	void SetOverflowBehavior(BoxLayoutOverflowBehavior behavior) { m_overflowBehavior = behavior; }
	BoxLayoutOverflowBehavior OverflowBehavior() const { return m_overflowBehavior; }

	/// Take ownership of the item
	void AddItem(UiElement *item) {
		Items().push_back(item);
	}
	/// Hand ownership of the item
	UiElement *RemoveItem() {
		if (Items().size() == 0) {
			return NULL;
		}
		UiElement *item = Items().back();
		Items().pop_back();
		return item;
	}

	/// Automatically infer size hint from content.
	void AutoSizeHint() {
		int sum = 0;
		int max = 0;
		for (auto item : Items()) {
			const ::Rect & rect = item->SizeHint();
#ifdef BUI_HBOX_IMPLEMENTATION
			sum += rect.w;
			max = std::max(max, rect.h);
#else
			sum += rect.h;
			max = std::max(max, rect.w);
#endif
			/* TODO: design issue: unable to determine whether a null size is a forced null size or a not hinted size
			if (rect.IsNull()) {
				SetSizeHint(::Rect());
				return;
			}
			*/
		}

#ifdef BUI_HBOX_IMPLEMENTATION
		SetInnerSizeHint(0, 0, sum, max);
#else
		SetInnerSizeHint(0, 0, max, sum);
#endif
	}

public:
	void Update() override {
		/* for regular items
		int itemHeight = floor(Rect().h / m_items.size());
		// Prevent rounding issues
		int lastItemHeight = Rect().h - (m_items.size() - 1) * itemHeight;

		for (size_t i = 0 ; i < m_items.size() ; ++i) {
		m_items[i]->SetRect(Rect().x, Rect().y + i * itemHeight, Rect().w, i == (m_items.size() - 1) ? lastItemHeight : itemHeight);
		}
		//*/

		int sumVHints = 0;
		int nullHints = 0;
		for (auto item : Items()) {
			const ::Rect & rect = item->SizeHint();
#ifdef BUI_HBOX_IMPLEMENTATION
			sumVHints += rect.w;
#else
			sumVHints += rect.h;
#endif
			if (rect.IsNull()) {
				nullHints++;
			}
		}
		int nonNullHints = static_cast<int>(Items().size()) - nullHints;
#ifdef BUI_HBOX_IMPLEMENTATION
		int remainingHeight = InnerRect().w - sumVHints;
#else
		int remainingHeight = InnerRect().h - sumVHints;
#endif
		int itemHeight = nullHints == 0 ? 0 : std::max(0, (int)floor(remainingHeight / nullHints));
		int lastItemHeight = std::max(0, remainingHeight) - (nullHints - 1) * itemHeight;
		int hintedItemHeightDelta = nonNullHints == 0 ? 0 : std::min((int)floor(remainingHeight / nonNullHints), 0);
		// The last height might be a bit different to prevent rounding issues
		int lastHintedItemHeightDelta = -std::max(0, -remainingHeight) + (nonNullHints - 1) * hintedItemHeightDelta;

		if (m_overflowBehavior == BoxLayoutOverflowBehavior::HideOverflow) {
			hintedItemHeightDelta = 0;
			lastHintedItemHeightDelta = 0;
		}

		int offset = 0;
		int nullCount = 0;
		int nonNullCount = 0;
		for (auto item : Items()) {
			int height = 0;
			const ::Rect & hint = item->SizeHint();
			if (hint.IsNull()) {
				height = nullCount == nullHints - 1 ? lastItemHeight : itemHeight;
				nullCount++;
			}
			else {
#ifdef BUI_HBOX_IMPLEMENTATION
				height = hint.w;
#else
				height = hint.h;
#endif
				height += (nonNullCount == nonNullHints - 1 ? lastHintedItemHeightDelta : hintedItemHeightDelta);
				nonNullCount++;
			}
#ifdef BUI_HBOX_IMPLEMENTATION
			item->SetRect(InnerRect().x + offset, InnerRect().y, height, InnerRect().h);
#else
			item->SetRect(InnerRect().x, InnerRect().y + offset, InnerRect().w, height);
#endif
			item->Update();
			offset += height;
		}
	}

protected:
	bool GetIndexAt(size_t & idx, int x, int y) override {
		/* for regular items
		int itemHeight = Rect().h / m_items.size();
		size_t i = std::min((size_t)(floor((y - Rect().y) / itemHeight)), m_items.size() - 1);
		m_items[i]->QueryAt(x, y);
		*/

		int offset = 0;
		for (size_t i = 0; i < Items().size(); ++i) {
#ifdef BUI_HBOX_IMPLEMENTATION
			offset += Items()[i]->InnerRect().w;
			if (InnerRect().x + offset > x) {
#else
			offset += Items()[i]->InnerRect().h;
			if (InnerRect().y + offset > y) {
#endif
				idx = i;
				return true;
			}
		}

		return false;
	}

private:
	BoxLayoutOverflowBehavior m_overflowBehavior;
};
