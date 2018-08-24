#include "UiIntInput.h"

void UiIntInput::SetValue(int value) {
	SetText(std::to_string(value));
}

int UiIntInput::Value() const {
	return std::stoi(Text());
}

void UiIntInput::OnChar(unsigned int codepoint) {
	// Filter out non integer characters
	if (codepoint >= '0' && codepoint <= '9') {
		UiTextInput::OnChar(codepoint);
	}
}
