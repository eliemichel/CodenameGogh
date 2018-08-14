#include "mainGui.h"

#include "Ui/UiApp.h"
#include "Ui/UiWindow.h"
#include "Ui/MainLayout.h"
#include "Ui/UiBase.h"

#include <GLFW/glfw3.h>
#include <nanovg.h>

#include "Ui/ExtraUi.hpp"

class NodeArea : public UiTrackMouseElement {
public:

public: // protected
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
		m_mouseX = x;
		m_mouseY = y;

		if (m_isMovingNode) {
			m_nodeX = m_moveStartNodeX + m_mouseX - m_moveStartMouseX;
			m_nodeY = m_moveStartNodeY + m_mouseY - m_moveStartMouseY;
		}
	}

	void OnMouseClick(int button, int action, int mods) override {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			const ::Rect & r = InnerRect();
			const ::Rect & nodeRect = ::Rect(r.x + m_nodeX, r.y + m_nodeY, 200, 100);

			if (nodeRect.Contains(m_mouseX, m_mouseY)) {
				m_moveStartMouseX = m_mouseX;
				m_moveStartMouseY = m_mouseY;
				m_moveStartNodeX = m_nodeX;
				m_moveStartNodeY = m_nodeY;
				m_isMovingNode = true;
			}
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			m_isMovingNode = false;
		}
	}

private:
	int m_mouseX;
	int m_mouseY;
	float m_nodeX;
	float m_nodeY;

	int m_moveStartMouseX;
	int m_moveStartMouseY;
	int m_moveStartNodeX;
	int m_moveStartNodeY;
	bool m_isMovingNode = false;
};

int mainGui(const ArgParse & args)
{
	UiApp app;
	UiWindow window(&app);

	//MainLayout *popupLayout = new MainLayout();
	//window.SetContent(popupLayout);

	UiVBoxLayout *layout = new UiVBoxLayout();

	TextButton *button = new TextButton();
	button->SetText("Test 1");
	layout->AddItem(button);

	button = new TextButton();
	button->SetText("Test 2");
	layout->AddItem(button);

	//popupLayout->AddItem(layout);
	//popupLayout->SetRect(0, 0, window.Width(), window.Height());
	NodeArea *nodeArea = new NodeArea();
	nodeArea->SetRect(0, 0, WIDTH, HEIGHT);
	window.SetContent(nodeArea);

	while (!window.ShouldClose())
	{
		window.Render();
		window.Poll();
	}

	return EXIT_SUCCESS;
}
