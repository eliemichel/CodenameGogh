#ifndef H_UIMENU
#define H_UIMENU

#include "UiBase.h"
#include "UiButton.h"

class UiMenu : public UiVBoxLayout {
public: // protected:
	void Paint(NVGcontext *vg) const override {
		UiElement::Paint(vg);
		const ::Rect & r = InnerRect();

		// Background
		nvgBeginPath(vg);
		nvgRect(vg, r.xf(), r.yf(), r.wf(), r.hf());
		nvgFillColor(vg, nvgRGB(56, 57, 58));
		nvgFill(vg);

		UiVBoxLayout::Paint(vg);
	}
};

class UiMenuButtonItem : public UiButton {
};

#endif // H_UIMENU
