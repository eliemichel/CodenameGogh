#ifndef H_UICHECKBOX
#define H_UICHECKBOX

#include "UiBase.h"
#include "Signal.h"
#include <string>

class UiCheckbox : public UiTrackMouseElement {
public:
	void SetChecked(bool checked) { m_checked = checked; }
	const bool & IsChecked() const { return m_checked; }

public: // protected:
	void OnMouseClick(int button, int action, int mods) override;

	void Paint(NVGcontext *vg) const override;

public: // signals
	Signal<bool> changed;

private:
	bool m_checked;
};

#endif // H_UICHECKBOX
