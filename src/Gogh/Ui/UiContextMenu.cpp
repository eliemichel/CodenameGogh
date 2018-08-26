#include "UiContextMenu.h"
#include "UiMenu.h"

void UiContextMenu::Popup(int x, int y) {
	if (!m_popupLayout) {
		return;
	}

	UiMenu *menu = new UiMenu();

	for (int i = 0; i < m_itemLabels.size(); ++i) {
		UiMenuButtonItem *button = new UiMenuButtonItem();
		button->SetText(m_itemLabels[i]);
		button->SetInnerSizeHint(0, 0, 0, 30);
		menu->AddItem(button);
	}

	menu->AutoSizeHint();
	const ::Rect & rect = menu->SizeHint();
	menu->SetRect(x, y, 150, rect.h);

	m_popupLayout->AddItem(menu);
	m_popupLayout->Update();
}
