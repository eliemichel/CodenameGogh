#ifndef H_UIENUMINPUT
#define H_UIENUMINPUT

#include "UiBase.h"

#include <vector>
#include <string>

class UiEnumInput : public UiTrackMouseElement {
public:
	/// The popupLayout is the layout to which the popup menu is spawned
	UiEnumInput(UiLayout *popupLayout = nullptr);

	int CurrentItemIndex() const { return m_currentItemIndex; }
	void SetCurrentItemIndex(int index);

	void SetItemLabels(const std::vector<std::string> & itemLabels) { m_itemLabels = itemLabels; }

public: // protected:
	void OnMouseClick(int button, int action, int mods) override;
	void Paint(NVGcontext *vg) const override;

private:
	UiLayout * m_popupLayout;
	UiElement * m_popupElement;
	int m_currentItemIndex;
	std::vector<std::string> m_itemLabels;
};

#endif // H_UIENUMINPUT
