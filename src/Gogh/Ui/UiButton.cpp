#include "UiButton.h"

#include <GLFW/glfw3.h>

void UiButton::PaintLabel(NVGcontext *vg) const {
	const ::Rect & r = InnerRect();
	nvgTextAlign(vg, NVG_ALIGN_CENTER);
	nvgTextLineHeight(vg, 13.0f / 15.0f);
	nvgFillColor(vg, nvgRGB(60, 60, 60));
	nvgTextBox(vg, r.x + 2, r.y + r.h - 6, r.w - 4, Text().c_str(), NULL);
}

void UiButton::Paint(NVGcontext *vg) const {
	const ::Rect & r = InnerRect();
	bool isMouseOver = IsMouseOver();

	// Background
	nvgBeginPath(vg);
	nvgRect(vg, r.x, r.y, r.w, r.h);
	nvgFillColor(vg, isMouseOver ? nvgRGB(213, 230, 247) : nvgRGB(232, 239, 247));
	nvgFill(vg);
	// Main Border
	nvgBeginPath(vg);
	nvgRect(vg, r.x + 0.5, r.y + 0.5, r.w - 1, r.h - 1);
	nvgStrokeColor(vg, isMouseOver ? nvgRGB(122, 176, 231) : nvgRGB(232, 239, 247));
	nvgStroke(vg);

	PaintLabel(vg);
}
