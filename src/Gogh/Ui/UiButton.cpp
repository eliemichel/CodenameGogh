#include "UiButton.h"

#include <GLFW/glfw3.h>

void UiButton::PaintLabel(NVGcontext *vg) const {
	const ::Rect & r = InnerRect();
	nvgTextAlign(vg, NVG_ALIGN_CENTER);
	nvgTextLineHeight(vg, 13.0f / 15.0f);
	nvgFillColor(vg, nvgRGB(60, 60, 60));
	nvgTextBox(vg, r.xf() + 2.f, r.yf() + r.hf() - 6.f, r.wf() - 4.f, Text().c_str(), NULL);
}

void UiButton::Paint(NVGcontext *vg) const {
	const ::Rect & r = InnerRect();
	bool isMouseOver = IsMouseOver();

	// Background
	nvgBeginPath(vg);
	nvgRect(vg, r.xf(), r.yf(), r.wf(), r.hf());
	nvgFillColor(vg, isMouseOver ? nvgRGB(213, 230, 247) : nvgRGB(232, 239, 247));
	nvgFill(vg);
	// Main Border
	nvgBeginPath(vg);
	nvgRect(vg, r.xf() + 0.5f, r.yf() + 0.5f, r.wf() - 1.f, r.hf() - 1.f);
	nvgStrokeColor(vg, isMouseOver ? nvgRGB(122, 176, 231) : nvgRGB(232, 239, 247));
	nvgStroke(vg);

	PaintLabel(vg);
}
