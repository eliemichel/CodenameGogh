#ifndef H_UIBUTTON
#define H_UIBUTTON

#include "UiBase.h"
#include "Signal.h"
#include <string>

class UiButton : public UiTrackMouseElement {
public:
	void SetText(const std::string & text) { m_text = text; }
	const std::string & Text() const { return m_text; }

protected:
	void PaintLabel(NVGcontext *vg) const;

public: // protected:
	void OnMouseClick(int button, int action, int mods) override;

	void Paint(NVGcontext *vg) const override;

public: // signals
	Signal<> clicked;
	
private:
	std::string m_text;
};

#endif // H_UIBUTTON
