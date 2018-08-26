#ifndef H_UICONTEXTMENU
#define H_UICONTEXTMENU

#include "UiBase.h"

#include <vector>
#include <string>

class UiContextMenu : public UiVBoxLayout {
public:
	UiContextMenu(UiLayout *popupLayout) : m_popupLayout(popupLayout) {}

	void SetItemLabels(const std::vector<std::string> & itemLabels) { m_itemLabels = itemLabels; }

	void Popup(int x, int y);

private:
	UiLayout * m_popupLayout;
	std::vector<std::string> m_itemLabels;
};


#endif // H_UICONTEXTMENU
