#include "UiTextInput.h"
#include "Logger.h"

#include <GLFW/glfw3.h>

UiTextInput::UiTextInput()
	: m_color(nvgRGB(0, 0, 0))
	, m_isEditing(false)
	, m_cursorTextIndex(3)
	, m_mustUpdateCursorPos(false)
{}

void UiTextInput::Paint(NVGcontext *vg) const {
	const ::Rect & r = InnerRect();

	// Work around a design issue: to get the position of the cursor from the
	// one of the mouse, we must have access to the text processor, which is in
	// the nanovg context
	// This forces m_cursorTextIndex to be mutable
	if (m_mustUpdateCursorPos) {
		m_cursorTextIndex = characterAtPos(vg, MouseX(), MouseY());
		m_mustUpdateCursorPos = false;
	}

	// Background
	nvgBeginPath(vg);
	nvgRect(vg, r.x, r.y, r.w, r.h);
	nvgFillColor(vg, nvgRGB(255, 255, 255));
	nvgFill(vg);

	// Border
	nvgBeginPath(vg);
	nvgRect(vg, r.x + 0.5, r.y + 0.5, r.w - 1.0, r.h - 1.0);
	nvgStrokeColor(vg, m_isEditing ? nvgRGB(128, 128, 255) : nvgRGB(64, 64, 64));
	nvgStroke(vg);

	// Prepare Text and Cursor
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
	nvgFillColor(vg, Color());
	float tx, ty;
	textPosition(tx, ty);

	// Cursor
	if (m_isEditing) {
		float bounds[4];
		nvgTextBounds(vg, tx, ty, Text().c_str(), Text().c_str() + m_cursorTextIndex, bounds);

		nvgBeginPath(vg);
		nvgRect(vg, bounds[2] - 1, bounds[1], 1, bounds[3] - bounds[1]);
		nvgFill(vg);
	}

	// Text
	nvgText(vg, tx, ty, Text().c_str(), NULL);
}

void UiTextInput::OnMouseClick(int button, int action, int mods) {
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		m_mustUpdateCursorPos = true;
		m_isEditing = true;
	}
}

void UiTextInput::OnKey(int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		const char* c = glfwGetKeyName(key, scancode);
		if (c) {
			if (m_cursorTextIndex != -1) {
				m_text.insert(m_cursorTextIndex, c);
				m_cursorTextIndex += static_cast<int>(strlen(c));
			} else {
				m_text += c;
			}
		}
	}
}

void UiTextInput::OnChar(unsigned int codepoint) {
	// TODO: do a proper convertion, and use unicode in m_text
	std::u32string str = std::u32string(1, static_cast<char32_t>(codepoint));
	std::string res;
	for (auto c = str.begin(); c != str.end(); ++c) {
		char t = *c;
		res.push_back(t);
	}
	DEBUG_LOG << "codepoint: " << codepoint << " (" << res << ")";
}

void UiTextInput::OnDefocus() {
	m_isEditing = false;
}

void UiTextInput::textPosition(float & x, float & y) const {
	const ::Rect & r = InnerRect();
	x = r.x + 6.f;
	y = r.y + r.h / 2.f + 6.f;
}

int UiTextInput::characterAtPos(NVGcontext *vg, float x, float y) const {
	// TODO: use nvgTextGlyphPositions instead of repeated calls to nvgTextBounds
	float bounds[4];
	int pmid;
	int pmin = 0;
	int pmax = static_cast<int>(Text().size());
	const char *text = Text().c_str();
	float xafterpivot, tx, ty;
	textPosition(tx, ty);

	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);

	// Binary search
	while (pmin < pmax) {
		pmid = (pmin + pmax) / 2;
		nvgTextBounds(vg, tx, ty, text, text + pmid, bounds);
		xafterpivot = bounds[2];
		if (x <= xafterpivot) {
			pmax = pmid;
		} else {
			pmin = pmid + 1;
		}
	}

	// Final adjustment (rounding instead of ceiling the pos)
	if (pmin > 0) {
		nvgTextBounds(vg, tx, ty, text, text + pmin, bounds);
		xafterpivot = bounds[2];
		nvgTextBounds(vg, tx, ty, text, text + (pmin - 1), bounds);
		if (x - bounds[2] < xafterpivot - x) {
			pmin--;
		}
	}

	return pmin;
}
