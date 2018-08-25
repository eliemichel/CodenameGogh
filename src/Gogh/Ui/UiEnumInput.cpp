#include "UiEnumInput.h"
#include "UiButton.h"
#include "Logger.h"

#include <GLFW/glfw3.h>

class EnumMenu : public UiVBoxLayout {
public: // protected:
	void Paint(NVGcontext *vg) const override {
		UiElement::Paint(vg);
		const ::Rect & r = InnerRect();

		// Background
		nvgBeginPath(vg);
		nvgRect(vg, r.x, r.y, r.w, r.h);
		nvgFillColor(vg, nvgRGB(56, 57, 58));
		nvgFill(vg);

		UiVBoxLayout::Paint(vg);
	}
};

class EnumMenuItemButton : public UiButton {
public:
	EnumMenuItemButton(UiEnumInput * enumInput, int itemIndex)
		: m_enumInput(enumInput)
		, m_itemIndex(itemIndex)
	{}

public: // protected
	void OnMouseClick(int button, int action, int mods) override {
		UiButton::OnMouseClick(button, action, mods);

		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
			if (m_enumInput) {
				m_enumInput->SetCurrentItemIndex(m_itemIndex);
			}
		}
	}

private:
	UiEnumInput * m_enumInput;
	int m_itemIndex;
};

UiEnumInput::UiEnumInput(UiLayout *popupLayout)
	: m_popupLayout(popupLayout)
	, m_popupElement(nullptr)
	, m_currentItemIndex(-1)
{}

void UiEnumInput::SetCurrentItemIndex(int index) {
	if (index < -1 || index >= m_itemLabels.size()) {
		return;
	}
	m_currentItemIndex = index;

	// Close popup
	if (m_popupElement && m_popupLayout->RemoveItem(m_popupElement)) {
		delete m_popupElement;
	}
	m_popupElement = nullptr;
}

void UiEnumInput::OnMouseClick(int button, int action, int mods) {
	if (!m_popupLayout) {
		return;
	}

	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		if (m_popupElement && m_popupLayout->RemoveItem(m_popupElement)) {
			delete m_popupElement;
			m_popupElement = nullptr;
		}
		else {
			EnumMenu *enumMenu = new EnumMenu();

			for (int i = 0; i < m_itemLabels.size(); ++i) {
				EnumMenuItemButton *button = new EnumMenuItemButton(this, i);
				button->SetText(m_itemLabels[i]);
				button->SetInnerSizeHint(0, 0, 0, 30);
				enumMenu->AddItem(button);
			}

			enumMenu->AutoSizeHint();
			const ::Rect & rect = enumMenu->SizeHint();
			const ::Rect & r = InnerRect();
			enumMenu->SetRect(r.x, r.y + r.h, r.w, rect.h);

			m_popupElement = enumMenu;
			m_popupLayout->AddItem(m_popupElement);
			m_popupLayout->Update();
		}
	}
}

void UiEnumInput::Paint(NVGcontext *vg) const {
	UiElement::Paint(vg);
	const ::Rect & r = InnerRect();

	nvgBeginPath(vg);
	nvgRect(vg, r.x, r.y, r.w, r.h);
	nvgFillColor(vg, nvgRGB(80, 257, 191));
	nvgFill(vg);

	const char *text = m_currentItemIndex > -1 ? m_itemLabels[m_currentItemIndex].c_str() : "--";
	nvgTextAlign(vg, NVG_ALIGN_CENTER);
	nvgTextLineHeight(vg, 13.0f / 15.0f);
	nvgFillColor(vg, nvgRGB(255, 255, 255));
	nvgTextBox(vg, r.x + 2, r.y + r.h - 6 - 11, r.w - 4, text, NULL);
}
