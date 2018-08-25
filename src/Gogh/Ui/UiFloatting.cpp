#include "UiFloatting.h"

#include "UiWindow.h"

UiFloatting::UiFloatting()
	: m_window(nullptr)
{
}

void UiFloatting::Exec() {
	m_window = new UiWindow(200, 500, "", MenuWindow);
}

UiFloatting::~UiFloatting() {
	if (m_window) {
		delete m_window;
	}
}
