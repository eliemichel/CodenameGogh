#include "UiBase.h"

class MainLayout : public PopupStackLayout {
public: // protected
	void OnMouseClick(int button, int action, int mods) override {
		bool closeSizePopup = false;
		if (action == GLFW_PRESS) {
			// When one clicks on the background element, all popups are destroyed
			if (!Items().empty() && MouseFocusItem() == Items()[0]) {
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

private:
	bool m_isSizePopupOpened;
};
