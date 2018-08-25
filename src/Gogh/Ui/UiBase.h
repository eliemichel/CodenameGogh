/**
 * Paint Portable
 * Copyright (c) 2018 - Elie Michel
 */

#ifndef H_BASE_UI
#define H_BASE_UI

#include <glad/glad.h>
#include <nanovg.h>

#include <string>
#include <vector>
#include <algorithm>

#include <GLFW/glfw3.h>

struct Rect {
	int x, y, w, h;

	Rect() : x(0), y(0), w(0), h(0) {}
	Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}

	bool Contains(int _x, int _y) const {
		return _x >= x && _y >= y && _x < x + w && _y < y + h;
	}
	bool IsNull() const {
		return x == 0 && y == 0 && w == 0 && h == 0;
	}
};

class UiElement {
public:
	UiElement() : m_parent(nullptr) {}
	virtual ~UiElement() {

	}

	// Getters / Setters

	void SetRect(Rect rect) {
		m_rect = rect;
		m_innerRect.x = m_rect.x + m_margin.x;
		m_innerRect.y = m_rect.y + m_margin.y;
		m_innerRect.w = m_rect.w - m_margin.x - m_margin.w;
		m_innerRect.h = m_rect.h - m_margin.y - m_margin.h;
		Update();
	}
	void SetRect(int x, int y, int w, int h) { SetRect(::Rect(x, y, w, h)); }
	const Rect & Rect() const { return m_rect; }

	/// Return content rect, ie rect minus margin
	const ::Rect & InnerRect() const { return m_innerRect; }

	void SetMargin(::Rect margin) { m_margin = margin; Update(); }
	void SetMargin(int x, int y, int w, int h) { SetMargin(::Rect(x, y, w, h)); }
	const ::Rect & Margin() const { return m_margin; }

	void SetSizeHint(::Rect hint) {
		m_sizeHint = hint;
	}
	void SetSizeHint(int x, int y, int w, int h) {
		SetSizeHint(::Rect(x, y, w, h));
	}
	/// Beware to setup margins before
	void SetInnerSizeHint(::Rect hint) {
		SetSizeHint(
			hint.x,
			hint.y,
			hint.w + m_margin.x + m_margin.w,
			hint.h + m_margin.y + m_margin.h
		);
	}
	void SetInnerSizeHint(int x, int y, int w, int h) {
		SetInnerSizeHint(::Rect(x, y, w, h));
	}
	virtual ::Rect SizeHint() {
		return m_sizeHint;
	}

	/**
	 * Parent must always be a valid UiElement or nullptr. Containers are
	 * responsible for setting it when adding elements and resetting it to
	 * nullptr when removing the element. A container must not accept a UiElement
	 * that already has a parent.
	 */
	void SetParent(UiElement *parent) {
		if (m_parent == parent) {
			return;
		}
		m_parent = parent;
	}
	UiElement * Parent() const {
		return m_parent;
	}

	/**
	 * Returns the oldest non null ancestor, ie parent of parent of parent etc.
	 * If parent is null, simply returns this UiElement itself
	 */
	UiElement * RootParent() {
		UiElement *el = this;
		while (el->Parent()) {
			el = el->Parent();
		}
		return el;
	}

public: // protected:
	virtual void OnMouseOver(int x, int y) {}

	virtual void OnMouseClick(int button, int action, int mods) {}

	/// This is called only on the focused element
	virtual void OnKey(int key, int scancode, int action, int mods) {}

	virtual void OnChar(unsigned int codepoint) {}

	/// Called whenever the mouse moved anywhere, before OnMouseOver might be called
	/// This is used to keep track of when mouse comes in and gets out
	/// TODO: avoid dispatching to absolutely every object, only send to ones touched by the last mouse move.
	virtual void ResetMouse() {}

	virtual void ResetDebug() {
		m_debug = false;
	}

	/// Call when geometry gets updated
	/// MUST NOT call this->SetRect() or there would be infinite loops
	virtual void Update() {}

	virtual void OnTick(float time) {}

	virtual void Paint(NVGcontext *vg) const {
		PaintDebug(vg);
	}
	virtual void PaintDebug(NVGcontext *vg) const {
		if (m_debug && false) {
			nvgBeginPath(vg);
			nvgRect(vg, Rect().x, Rect().y, Rect().w, Rect().h);
			nvgFillColor(vg, nvgRGBA(255, 0, 0, 64));
			nvgFill(vg);
		}
	}

	virtual void OnDefocus() {}

private:
	UiElement *m_parent;
	::Rect m_rect, m_sizeHint, m_innerRect, m_margin;
	bool m_debug;
};

/**
 * Adds OnMouseEnter() and OnMouseLeave() events
 */
class UiMouseAwareElement : public UiElement {
public:
	UiMouseAwareElement()
		: UiElement()
		, m_isMouseOver(false)
		, m_wasMouseOver(false)
	{}

public: // protected:
	void OnMouseOver(int x, int y) override {
		UiElement::OnMouseOver(x, y);
		if (InnerRect().Contains(x, y)) {
			m_isMouseOver = true;
		}
	}

	void ResetMouse() override {
		UiElement::ResetMouse();
		if (m_isMouseOver && !m_wasMouseOver) {
			OnMouseEnter();
		}
		if (!m_isMouseOver && m_wasMouseOver) {
			OnMouseLeave();
		}

		m_wasMouseOver = m_isMouseOver;
		m_isMouseOver = false;
	}

	virtual void OnMouseEnter() {
	}

	virtual void OnMouseLeave() {
	}

protected:
	bool IsMouseOver() const { return m_isMouseOver; }

private:
	bool m_isMouseOver, m_wasMouseOver;
};

/**
 * Add MouseX() and MouseY() to UiMouseAwareElement
 * TODO: avoid storing this information in every single uielement that need it
 * since it is the same for everybody.
 */
class UiTrackMouseElement : public UiMouseAwareElement {
public:
	UiTrackMouseElement()
		: UiMouseAwareElement()
		, m_mouseX(0)
		, m_mouseY(0)
	{}

public: // protected:
	void OnMouseOver(int x, int y) override {
		UiMouseAwareElement::OnMouseOver(x, y);
		m_mouseX = x;
		m_mouseY = y;
	}

protected:
	int MouseX() const { return m_mouseX; }
	int MouseY() const { return m_mouseY; }

private:
	int m_mouseX, m_mouseY;
};

/**
 * Generic layout, that must be an ancestor of any element containing others,
 * because it handles the dispatching of events.
 * When inheriting, override at least Update() and GetIndexAt().
 * Warning: Do NOT insert a UiElement twice in a layout, neither in two
 * different layouts.
 * There are two notions of focus: the over focus, tracking over which the
 * mouse moved, which is reset at each frame, and the click focus, which is not
 * cleared and keeps track of the lastly clicked item to forward it key events.
 * An extra "press focus" is used to ensure that mouse release is dispatched to
 * elements where mouse press was sent.
 */ 
class UiLayout : public UiElement {
public:
	UiLayout()
		: UiElement()
		, m_mouseOverFocusIdx(-1)
		, m_mouseClickFocusIdx(-1)
		, m_mousePressFocusIdx(-1)
	{}

	~UiLayout() {
		while (!m_items.empty()) {
			delete m_items.back();
			m_items.pop_back();
		}
	}

	/// Take ownership of the item. Returns false if the item is null or already has a parent (and doesn't add it)
	bool AddItem(UiElement *item) {
		if (!item || item->Parent()) {
			return false;
		}
		item->SetParent(this);
		m_items.push_back(item);
		return true;
	}
	/// Give back ownership of the item
	UiElement *RemoveItem() {
		if (m_items.empty()) {
			return nullptr;
		}

		// CLear focus before removing the element
		if (m_mouseClickFocusIdx >= m_items.size() - 1) {
			SetClickFocus(-1);
		}

		// Pop the last element out
		UiElement *item = m_items.back();
		m_items.pop_back();

		// Clear element
		if (item) {
			item->SetParent(nullptr);
		}
		return item;
	}
	/// Give back ownership of the item
	bool RemoveItem(UiElement *item) {
		auto it = m_items.begin();
		for (; it != m_items.end(); ++it) {
			if (*it == item) {
				// Clear focus before removing the element, or shift focusIdx
				// if it was targetting an element placed after the removed one
				int index = static_cast<int>(it - m_items.begin());
				if (m_mouseClickFocusIdx == index) {
					SetClickFocus(-1);
				} else if (m_mouseClickFocusIdx > index) {
					m_mouseClickFocusIdx--;
				}

				// Remove the element
				m_items.erase(it);

				// Clear element
				if (item) {
					item->SetParent(nullptr);
				}
				return true;
			}
		}
		return false;
	}

	size_t ItemCount() const {
		return Items().size();
	}

public: // protected:
	void OnMouseOver(int x, int y) override {
		UiElement::OnMouseOver(x, y);
		size_t i;
		if (GetIndexAt(i, x, y)) {
			Items()[i]->OnMouseOver(x, y);
			m_mouseOverFocusIdx = static_cast<int>(i);
		}
	}

	void OnMouseClick(int button, int action, int mods) override {
		// Forward click events to the move-focused item
		UiElement::OnMouseClick(button, action, mods);
		if (m_mouseOverFocusIdx > -1 && m_mouseOverFocusIdx < Items().size()) {
			Items()[m_mouseOverFocusIdx]->OnMouseClick(button, action, mods);
			SetClickFocus(m_mouseOverFocusIdx);
		}

		if (action == GLFW_PRESS) {
			m_mousePressFocusIdx = m_mouseOverFocusIdx;
		} else if (action == GLFW_RELEASE && m_mousePressFocusIdx > -1 && m_mousePressFocusIdx < Items().size()) {
			Items()[m_mousePressFocusIdx]->OnMouseClick(button, action, mods);
			m_mousePressFocusIdx = -1;
		}
	}

	void OnKey(int key, int scancode, int action, int mods) override {
		// Forward key events to the click-focused item
		UiElement::OnKey(key, scancode, action, mods);
		if (m_mouseClickFocusIdx > -1 && m_mouseClickFocusIdx < Items().size()) {
			Items()[m_mouseClickFocusIdx]->OnKey(key, scancode, action, mods);
		}
	}

	void OnChar(unsigned int codepoint) override {
		// Forward key events to the click-focused item
		UiElement::OnChar(codepoint);
		if (m_mouseClickFocusIdx > -1 && m_mouseClickFocusIdx < Items().size()) {
			Items()[m_mouseClickFocusIdx]->OnChar(codepoint);
		}
	}

	void ResetMouse() override {
		UiElement::ResetMouse();
		for (auto item : Items()) {
			item->ResetMouse();
		}
		m_mouseOverFocusIdx = -1;
	}

	void ResetDebug() override {
		UiElement::ResetDebug();
		for (auto item : Items()) {
			item->ResetDebug();
		}
	}

	void OnTick(float time) override {
		UiElement::OnTick(time);
		for (UiElement *item : Items()) {
			item->OnTick(time);
		}
	}

	void Paint(NVGcontext *vg) const override {
		UiElement::Paint(vg);
		for (UiElement *item : Items()) {
			item->Paint(vg);
		}
		PaintDebug(vg);
	}

	void OnDefocus() override {
		SetClickFocus(-1);
	}

protected:
	std::vector<UiElement*> & Items() { return m_items; }
	const std::vector<UiElement*> & Items() const { return m_items; }

	int MouseFocusIdx() const { return m_mouseOverFocusIdx; }

	virtual bool GetIndexAt(size_t & idx, int x, int y) {
		return false;
	}

private:
	void SetClickFocus(int idx) {
		// If item already has focus, change nothing
		if (m_mouseClickFocusIdx == idx) {
			return;
		}

		if (m_mouseClickFocusIdx >= 0 && m_mouseClickFocusIdx < Items().size()) {
			Items()[m_mouseClickFocusIdx]->OnDefocus();
		}

		m_mouseClickFocusIdx = idx;

		if (m_mouseClickFocusIdx >= 0 && m_mouseClickFocusIdx < Items().size()) {
			// There is no need for an "OnFocus", elements just assume that
			// they got it as soon as they receive a OnClick event.
			//Items()[m_mouseClickFocusIdx]->OnFocus();
		} else {
			m_mouseClickFocusIdx = -1;
		}
	}

private:
	std::vector<UiElement*> m_items;
	int m_mouseOverFocusIdx;
	int m_mouseClickFocusIdx;
	int m_mousePressFocusIdx;
};

// Item 0 is the background, other items are stacked popups
class PopupStackLayout : public UiLayout {
public: // protected:
	void Update() override {
		const ::Rect & r = InnerRect();
		if (Items().size() > 0) {
			Items()[0]->SetRect(r);
		}
	}

protected:
	bool GetIndexAt(size_t & idx, int x, int y) override {
		const ::Rect & r = InnerRect();
		if (Items().size() == 0) {
			return false;
		}
		for (size_t i = Items().size() - 1; i > 0; --i) {
			if (Items()[i]->Rect().Contains(x, y)) {
				idx = i;
				return true;
			}
		}
		idx = 0;
		return true;
	}
};

class GridLayout : public UiLayout {
public:
	GridLayout()
		: m_rowCount(1)
		, m_colCount(1)
		, m_rowSpacing(0)
		, m_colSpacing(0)
	{}

	void SetRowCount(int count) { m_rowCount = std::max(1, count); }
	int RowCount() const { return m_rowCount; }

	void SetColCount(int count) { m_colCount = std::max(1, count); }
	int ColCount() const { return m_colCount; }

	void SetRowSpacing(int spacing) { m_rowSpacing = spacing; }
	int RowSpacing() const { return m_rowSpacing; }

	void SetColSpacing(int spacing) { m_colSpacing = spacing; }
	int ColSpacing() const { return m_colSpacing; }

public: // protected:
	void Update() override {
		const ::Rect & r = InnerRect();
		int itemWidth = (r.w - ColSpacing() * (ColCount() - 1)) / ColCount() + ColSpacing();
		int itemHeight = (r.h - RowSpacing() * (RowCount() - 1)) / RowCount() + RowSpacing();
		// Prevent rounding issues
		int lastItemWidth = r.w - (ColCount() - 1) * itemWidth;
		int lastItemHeight = r.h - (RowCount() - 1) * itemHeight;

		for (size_t i = 0 ; i < Items().size() ; ++i) {
			int colIndex = static_cast<int>(i) % ColCount();
			int rowIndex = static_cast<int>(i) / ColCount();
			Items()[i]->SetRect(
				r.x + colIndex * itemWidth,
				r.y + rowIndex * itemHeight,
				colIndex == (ColCount() - 1) ? lastItemWidth : (itemWidth - ColSpacing()),
				rowIndex == (RowCount() - 1) ? lastItemHeight : (itemHeight - RowSpacing())
			);
		}
	}

protected:
	bool GetIndexAt(size_t & idx, int x, int y) override {
		const ::Rect & r = InnerRect();
		int relativeX = x - r.x;
		int relativeY = y - r.y;

		int itemWidth = (r.w - ColSpacing() * (ColCount() - 1)) / ColCount() + ColSpacing();
		int itemHeight = (r.h - RowSpacing() * (RowCount() - 1)) / RowCount() + RowSpacing();

		size_t colIndex = std::min((int)(floor(relativeX / itemWidth)), ColCount() - 1);
		size_t rowIndex = std::min((int)(floor(relativeY / itemHeight)), RowCount() - 1);

		bool isInColSpacing = (colIndex + 1) * itemWidth - relativeX <= ColSpacing();
		bool isInRowSpacing = (rowIndex + 1) * itemHeight - relativeY <= RowSpacing();

		if (!isInRowSpacing && !isInColSpacing) {
			idx = rowIndex * ColCount() + colIndex;
			return idx < Items().size();
		}
		else {
			return false;
		}
	}

private:
	int m_rowCount, m_colCount;
	int m_rowSpacing, m_colSpacing;
};

typedef enum {
	ShrinkOverflow,
	HideOverflow,
} BoxLayoutOverflowBehavior;

#define BUI_HBOX_IMPLEMENTATION
#include "UiBoxLayout.inc.h"
#undef BUI_HBOX_IMPLEMENTATION
#include "UiBoxLayout.inc.h"

class UiLabel : public UiElement {
public:
	UiLabel() : m_color(nvgRGBA(255, 255, 255, 255)) {}

	void SetText(const std::string & text) { m_text = text; }
	const std::string & Text() const { return m_text; }

	void SetColor(int r, int g, int b, int a = 255) { m_color = nvgRGBA(r, g, b, a); }
	void SetColor(NVGcolor color) { m_color = color; }
	const NVGcolor & Color() const { return m_color; }

public: // protected:
	void Paint(NVGcontext *vg) const override {
		const ::Rect & r = InnerRect();
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgFillColor(vg, Color());
		nvgText(vg, r.x + r.w / 2.f, r.y + r.h - 6.f, Text().c_str(), NULL);
	}

private:
	std::string m_text;
	NVGcolor m_color;
};

#endif // H_BASE_UI