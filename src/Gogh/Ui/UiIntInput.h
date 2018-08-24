#ifndef H_UIINTINPUT
#define H_UIINTINPUT

#include "UiTextInput.h"

#include <string>

class UiIntInput : public UiTextInput {
public:
	void SetValue(int value);
	int Value() const;

public: // protected:
	void OnChar(unsigned int codepoint) override;

private:
	// Remove from public API
	void SetText(const std::string & text) { UiTextInput::SetText(text); }
	const std::string & Text() const { return UiTextInput::Text(); }
};

#endif // H_UIINTINPUT
