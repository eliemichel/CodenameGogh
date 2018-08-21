#ifndef H_EXTRAUI
#define H_EXTRAUI

// Custom UI elements

/// Add MouseX() and MouseY() to UiMouseAwareElement
/// TODO: avoid storing this information in every single uielement that need it since it is the same for everybody
class UiTrackMouseElement : public UiMouseAwareElement {
public:
	UiTrackMouseElement()
		: UiMouseAwareElement()
		, m_mouseX(0)
		, m_mouseY(0)
	{}

public: // protected
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

/// Blue-bordered buttons
class UiDefaultButton : public UiTrackMouseElement {
protected:
	/// To be overridden
	virtual bool IsCurrent() const { return false; }

public: // protected
	void Paint(NVGcontext *vg) const override {
		const ::Rect & r = InnerRect();
		bool isCurrent = IsCurrent();
		bool isMouseOver = IsMouseOver();

		if (isCurrent || isMouseOver) {
			// Background
			nvgBeginPath(vg);
			nvgRect(vg, r.x, r.y, r.w, r.h);
			nvgFillColor(vg, isCurrent ? (isMouseOver ? nvgRGB(213, 230, 247) : nvgRGB(201, 224, 247)) : nvgRGB(232, 239, 247));
			nvgFill(vg);
			// Main Border
			nvgBeginPath(vg);
			nvgRect(vg, r.x + 0.5, r.y + 0.5, r.w - 1, r.h - 1);
			nvgStrokeColor(vg, isCurrent ? (isMouseOver ? nvgRGB(122, 176, 231) : nvgRGB(98, 162, 228)) : nvgRGB(164, 206, 249));
			nvgStroke(vg);
		}
	}
};

/// Default button with a label on it
class TextButton : public UiDefaultButton {
public:
	void SetText(const std::string & text) { m_text = text; }
	const std::string & Text() const { return m_text; }

protected:
	void PaintLabel(NVGcontext *vg) const {
		const ::Rect & r = InnerRect();
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgTextLineHeight(vg, 13.0f / 15.0f);
		nvgFillColor(vg, nvgRGB(60, 60, 60));
		nvgTextBox(vg, r.x + 2, r.y + r.h - 6 - 11, r.w - 4, Text().c_str(), NULL);
	}

public: // protected
	void Paint(NVGcontext *vg) const override {
		UiDefaultButton::Paint(vg);
		PaintLabel(vg);
	}

private:
	std::string m_text;
};

#endif // H_EXTRAUI
