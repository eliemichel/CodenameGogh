#include "mainGui.h"

#include "Ui/UiApp.h"
#include "Ui/UiWindow.h"
#include "Ui/MainLayout.h"
#include "Ui/UiBase.h"
#include "Parameter.h"

#include "Logger.h"
#include "Ui/ExtraUi.hpp"
#include "Ui/ParameterWidget.h"
#include "Ui/UiTextInput.h"
#include "Ui/UiIntInput.h"
#include "Ui/UiFloatting.h"

#include <GLFW/glfw3.h>
#include <nanovg.h>

class NodeArea : public UiTrackMouseElement {
public: // protected:
	void Paint(NVGcontext *vg) const override {
		UiTrackMouseElement::Paint(vg);
		const ::Rect & r = InnerRect();
		
		// Background
		nvgBeginPath(vg);
		nvgRect(vg, r.x, r.y, r.w, r.h);
		nvgFillColor(vg, nvgRGB(30, 57, 91));
		nvgFill(vg);
		
		// Node
		nvgBeginPath(vg);
		nvgRect(vg, r.x + m_nodeX, r.y + m_nodeY, 200, 100);
		nvgFillColor(vg, nvgRGB(128, 57, 91));
		nvgFill(vg);
	}
	
	void OnMouseOver(int x, int y) override {
		UiTrackMouseElement::OnMouseOver(x, y);

		if (m_isMovingNode) {
			m_nodeX = m_moveStartNodeX + MouseX() - m_moveStartMouseX;
			m_nodeY = m_moveStartNodeY + MouseY() - m_moveStartMouseY;
		}
	}

	void OnMouseClick(int button, int action, int mods) override {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			const ::Rect & r = InnerRect();
			const ::Rect & nodeRect = ::Rect(r.x + m_nodeX, r.y + m_nodeY, 200, 100);

			if (nodeRect.Contains(MouseX(), MouseY())) {
				m_moveStartMouseX = MouseX();
				m_moveStartMouseY = MouseY();
				m_isMovingNode = true;
			}
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			m_isMovingNode = false;
		}
	}

private:
	float m_nodeX;
	float m_nodeY;

	int m_moveStartMouseX;
	int m_moveStartMouseY;
	int m_moveStartNodeX;
	int m_moveStartNodeY;
	bool m_isMovingNode = false;
};

class TestElement : public UiElement {
public:
	TestElement() {
		SetRect(0, 0, 150, 50);
	}

public: // protected:
	void Paint(NVGcontext *vg) const override {
		UiElement::Paint(vg);
		const ::Rect & r = InnerRect();

		DEBUG_LOG << "rect: " << r.x << ", " << r.y << ", " << r.w << ", " << r.h;

		nvgBeginPath(vg);
		nvgRect(vg, r.x, r.y, r.w, r.h);
		nvgFillColor(vg, nvgRGB(130, 57, 91));
		nvgFill(vg);
	}
};

class UiEnumInput : public UiTrackMouseElement {
public:
	UiEnumInput(UiLayout *popupLayout)
		: m_popupLayout(popupLayout)
		, m_popupElement(nullptr)
	{}

public: // protected:
	void OnMouseClick(int button, int action, int mods) override {
		if (!m_popupLayout) {
			return;
		}

		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
			if (m_popupElement && m_popupLayout->RemoveItem(m_popupElement)) {
				delete m_popupElement;
				m_popupElement = nullptr;
			} else {
				m_popupElement = new TestElement();
				m_popupLayout->AddItem(m_popupElement);
				m_popupLayout->Update();
			}
		}
	}

	void Paint(NVGcontext *vg) const override {
		UiElement::Paint(vg);
		const ::Rect & r = InnerRect();

		nvgBeginPath(vg);
		nvgRect(vg, r.x, r.y, r.w, r.h);
		nvgFillColor(vg, nvgRGB(80, 257, 191));
		nvgFill(vg);
	}

private:
	UiLayout * m_popupLayout;
	UiElement * m_popupElement;
};

int mainGui(const ArgParse & args)
{
	UiApp app;
	UiWindow window(1200, 600, "Gogh");

	MainLayout *popupLayout = new MainLayout();

	UiHBoxLayout *mainLayout = new UiHBoxLayout();

	// Node Area
	NodeArea *nodeArea = new NodeArea();
	mainLayout->AddItem(nodeArea);

	// Sidebar
	UiVBoxLayout *layout = new UiVBoxLayout();

	Parameter *param = new Parameter();
	param->setType(StringType);
	param->setName("Yo");
	param->set(QString::fromStdString("bloum"));
	ParameterWidget *paramWidget = new ParameterWidget();
	paramWidget->SetParameter(param);
	paramWidget->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(paramWidget);

	TextButton *button = new TextButton();
	button->SetText("Test 1");
	button->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(button);

	button = new TextButton();
	button->SetText("Test 2");
	button->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(button);

	UiTextInput *textInput = new UiTextInput();
	textInput->SetText("blabla");
	textInput->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(textInput);

	UiIntInput *intInput = new UiIntInput();
	intInput->SetValue(42);
	intInput->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(intInput);

	UiEnumInput *enumInput = new UiEnumInput(popupLayout);
	enumInput->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(enumInput);

	layout->SetInnerSizeHint(0, 0, 300, 0);
	mainLayout->AddItem(layout);

	popupLayout->AddItem(mainLayout);
	window.SetContent(popupLayout);
	popupLayout->SetRect(0, 0, 1200, 600);

	while (!window.ShouldClose())
	{
		window.Render();
		window.Poll();
	}

	return EXIT_SUCCESS;
}
