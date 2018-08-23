#ifndef H_UITEXTINPUT
#define H_UITEXTINPUT

#include "UiBase.h"
#include "ExtraUi.hpp"
#include <string>

class UiTextInput : public UiTrackMouseElement {
public:
	UiTextInput();

	void SetText(const std::string & text) { m_text = text; }
	const std::string & Text() const { return m_text; }

	void SetColor(int r, int g, int b, int a = 255) { m_color = nvgRGBA(r, g, b, a); }
	void SetColor(NVGcolor color) { m_color = color; }
	const NVGcolor & Color() const { return m_color; }

public: // protected:
	void Paint(NVGcontext *vg) const override;
	void OnMouseClick(int button, int action, int mods) override;
	void OnKey(int key, int scancode, int action, int mode) override;

	void OnDefocus() override;

private:
	/// Positioning of the text
	/// /!\ Only works with left aligned text
	void textPosition(float & x, float & y) const;
	/// Return the index of the character of m_text located at (x,y)
	int characterAtPos(NVGcontext *vg, float x, float y) const;

private:
	std::string m_text;
	NVGcolor m_color;
	bool m_isEditing;
	mutable int m_cursorTextIndex; // index 0 is *before* the first character
	mutable bool m_mustUpdateCursorPos;
};

#endif // H_UITEXTINPUT
