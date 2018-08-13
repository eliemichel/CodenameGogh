#include "UiBase.h"

class MainLayout : public PopupStackLayout {
public: // protected
	void OnMouseClick(int button, int action, int mods) override {
		bool closeSizePopup = false;
		if (action == GLFW_PRESS) {
			// When one clicks on the background element, all popups are destroyed
			if (MouseFocusIdx() <= 0) {
				closeSizePopup = m_isSizePopupOpened;
				while (Items().size() > 1) {
					delete RemoveItem();
				}
			}
		}

		PopupStackLayout::OnMouseClick(button, action, mods);

		if (closeSizePopup) {
			m_isSizePopupOpened = false;
		}
	}

	void Paint(struct NVGcontext *vg) const override {
		nvgBeginPath(vg);
		nvgRect(vg, 0, 24, 500, 92);
		nvgFillColor(vg, nvgRGBA(245, 246, 247, 255));
		nvgFill(vg);
	}

private:
	bool m_isSizePopupOpened;
};
