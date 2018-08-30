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
		m_cursorTextIndex = CharacterAtPos(vg, static_cast<float>(MouseX()), static_cast<float>(MouseY()));
		m_mustUpdateCursorPos = false;
	}

	// Background
	nvgBeginPath(vg);
	nvgRect(vg, r.xf(), r.yf(), r.wf(), r.hf());
	nvgFillColor(vg, nvgRGB(255, 255, 255));
	nvgFill(vg);

	// Border
	nvgBeginPath(vg);
	nvgRect(vg, r.xf() + 0.5f, r.yf() + 0.5f, r.wf() - 1.f, r.h - 1.f);
	nvgStrokeColor(vg, m_isEditing ? nvgRGB(128, 128, 255) : nvgRGB(64, 64, 64));
	nvgStroke(vg);

	// Prepare Text and Cursor
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
	nvgFillColor(vg, Color());
	float tx, ty;
	TextPosition(tx, ty);

	// Cursor
	if (m_isEditing) {
		if (m_cursorBlink) {
			float bounds[4];
			nvgTextBounds(vg, tx, ty, DisplayText().c_str(), DisplayText().c_str() + m_cursorTextIndex, bounds);
			if (m_cursorTextIndex == 0) {
				bounds[2]++; // cosmetic fix
			}

			nvgBeginPath(vg);
			nvgRect(vg, bounds[2] - 1, bounds[1], 1, bounds[3] - bounds[1]);
			nvgFill(vg);
		}
	}

	// Text
	nvgText(vg, tx, ty, DisplayText().c_str(), NULL);
}

void UiTextInput::OnTick(float time) {
	if (m_isEditing) {
		if (m_cursorBlinkStart == -1.f) {
			m_cursorBlinkStart = time;
		}
		m_cursorBlink = (static_cast<int>(m_cursorBlinkStart * 2.f) % 2) == 1;
		m_cursorBlink = true;
	}
}

void UiTextInput::OnMouseClick(int button, int action, int mods) {
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		m_mustUpdateCursorPos = true;
		StartEditing();
	}
}

void UiTextInput::OnKey(int key, int scancode, int action, int mods) {
	// TODO Prevent from placing cursor index between two chars of a given utf8 codepoint!!
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_LEFT:
			m_cursorTextIndex = std::max(0, m_cursorTextIndex - 1);
			// TODO: ctrl+arrow: move by one word
			// missing: isAlphaNumeric for utf chars (maybe use http://site.icu-project.org/)
			/*
			if (mods & GLFW_MOD_CONTROL) {
				while (m_cursorTextIndex > 0 && isAlphaNumeric(m_editText[m_cursorTextIndex - 1])) {
					m_cursorTextIndex = std::max(0, m_cursorTextIndex - 1);
				}
			}
			*/
			break;

		case GLFW_KEY_RIGHT:
			m_cursorTextIndex = std::min(m_cursorTextIndex + 1, static_cast<int>(m_editText.size()));
			break;

		case GLFW_KEY_BACKSPACE:
			if (m_cursorTextIndex > 0) {
				m_cursorTextIndex--;
				m_editText.erase(m_editText.begin() + m_cursorTextIndex);
			}
			break;

		case GLFW_KEY_DELETE:
			if (m_cursorTextIndex < m_editText.size()) {
				m_editText.erase(m_editText.begin() + m_cursorTextIndex);
			}
			break;

		case GLFW_KEY_ENTER:
			SubmitEditing();
			break;

		case GLFW_KEY_ESCAPE:
			CancelEditing();
			break;

		case GLFW_KEY_END:
			m_cursorTextIndex = static_cast<int>(m_editText.size());
			break;

		case GLFW_KEY_HOME:
			m_cursorTextIndex = 0;
			break;

		case GLFW_KEY_TAB:
			// TODO: focus next element
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
		m_editText.insert(m_cursorTextIndex, utf8);
		m_cursorTextIndex += static_cast<int>(utf8.size());
	}
	else {
		m_editText += utf8;
	}
}

void UiTextInput::OnDefocus() {
	SubmitEditing();
}

void UiTextInput::StartEditing() {
	m_isEditing = true;
	m_cursorBlink = true;
	m_cursorBlinkStart = -1.f;
	m_editText = m_text;
}

void UiTextInput::SubmitEditing() {
	if (m_isEditing) {
		m_isEditing = false;
		m_text = m_editText;
		m_editText.clear();
		textEdited.fire(m_text);
	}
}

void UiTextInput::CancelEditing() {
	m_isEditing = false;
	m_editText.clear();
}

void UiTextInput::TextPosition(float & x, float & y) const {
	const ::Rect & r = InnerRect();
	x = r.x + 6.f;
	y = r.y + r.h / 2.f + 6.f;
}

int UiTextInput::CharacterAtPos(NVGcontext *vg, float x, float y) const {
	// TODO: use nvgTextGlyphPositions instead of repeated calls to nvgTextBounds
	float bounds[4];
	int pmid;
	int pmin = 0;
	int pmax = static_cast<int>(DisplayText().size());
	const char *text = DisplayText().c_str();
	float xafterpivot, tx, ty;
	TextPosition(tx, ty);

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
