#ifndef H_UITEXTINPUT
#define H_UITEXTINPUT

#include "UiBase.h"
#include "ExtraUi.hpp"
#include "Logger.h"
#include <string>

#include <GLFW/glfw3.h>

class UiTextInput : public UiMouseAwareElement {
public:
	UiTextInput()
		: m_color(nvgRGB(0, 0, 0))
		, m_isEditing(false)
	{}

	void SetText(const std::string & text) { m_text = text; }
	const std::string & Text() const { return m_text; }

	void SetColor(int r, int g, int b, int a = 255) { m_color = nvgRGBA(r, g, b, a); }
	void SetColor(NVGcolor color) { m_color = color; }
	const NVGcolor & Color() const { return m_color; }

public: // protected
	void Paint(NVGcontext *vg) const override {
		const ::Rect & r = InnerRect();

		// Background
		nvgBeginPath(vg);
		nvgRect(vg, r.x, r.y, r.w, r.h);
		nvgFillColor(vg, nvgRGB(255, 255, 255));
		nvgFill(vg);

		// Border
		nvgBeginPath(vg);
		nvgRect(vg, r.x+0.5, r.y+0.5, r.w-1.0, r.h-1.0);
		nvgStrokeColor(vg, m_isEditing ? nvgRGB(128, 128, 255) : nvgRGB(64, 64, 64));
		nvgStroke(vg);

		// Highlight
		float bounds[4];
		nvgTextBounds(vg, r.x + 2.0, r.y + r.h / 2.0 + 6, Text().c_str(), NULL, bounds);
		nvgBeginPath(vg);
		nvgRect(vg, bounds[0], bounds[1], bounds[2], bounds[3]);
		nvgFillColor(vg, m_isEditing ? nvgRGB(128, 128, 255) : nvgRGB(64, 64, 64));
		nvgFill(vg);

		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgFillColor(vg, Color());
		nvgText(vg, r.x + 2.0, r.y + r.h / 2.0 + 6, Text().c_str(), NULL);
	}

	void OnMouseClick(int button, int action, int mods) override {
		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
			m_isEditing = true;
		}
	}

private:
	std::string m_text;
	NVGcolor m_color;
	bool m_isEditing;
};

#endif // H_UITEXTINPUT
