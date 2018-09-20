#include "UiCheckbox.h"

void UiCheckbox::OnMouseClick(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		SetChecked(!IsChecked());
		changed.fire(IsChecked());
	}
}

void UiCheckbox::Paint(NVGcontext *vg) const {
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

	if (IsChecked()) {
		// Check
		nvgBeginPath(vg);
		nvgRect(vg, r.xf() + 4, r.yf() + 4, r.wf() - 8, r.hf() - 8);
		nvgFillColor(vg, nvgRGB(32, 32, 32));
		nvgFill(vg);
	}
}
