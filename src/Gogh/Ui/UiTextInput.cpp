#include "UiTextInput.h"
#include "Logger.h"

#include <GLFW/glfw3.h>

#include <locale>
#include <codecvt>

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
	// TODO Prevent from placing cursor index between two chars of a given utf8 codepoint
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_LEFT:
			m_cursorTextIndex = std::max(0, m_cursorTextIndex - 1);
			break;

		case GLFW_KEY_RIGHT:
			m_cursorTextIndex = std::min(m_cursorTextIndex + 1, static_cast<int>(m_text.size()));
			break;
		}
	}
}

void UiTextInput::OnChar(unsigned int codepoint) {
	std::u32string utf32 = std::u32string(1, static_cast<char32_t>(codepoint));

#ifdef _WIN32
	std::wstring wide_string(utf32.begin(), utf32.end());
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
	std::string utf8 = convert.to_bytes(wide_string);
#else // _WIN32
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	std::string utf8 = convert.to_bytes(utf32);
#endif // _WIN32
	
	if (m_cursorTextIndex != -1) {
		m_text.insert(m_cursorTextIndex, utf8);
		m_cursorTextIndex += utf8.size();
	}
	else {
		m_text += utf8;
	}
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