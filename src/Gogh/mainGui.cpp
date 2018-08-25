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
#include "Ui/UiEnumInput.h"
#include "Ui/UiButton.h"
#include "Ui/UiMenu.h"

#include <GLFW/glfw3.h>
#include <nanovg.h>

class UiContextMenu : public UiVBoxLayout {
public:
	UiContextMenu(UiLayout *popupLayout) : m_popupLayout(popupLayout) {}

	void SetItemLabels(const std::vector<std::string> & itemLabels) { m_itemLabels = itemLabels; }

	void Popup(int x, int y) {
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

private:
	UiLayout *m_popupLayout;
	std::vector<std::string> m_itemLabels;
};

class NodeArea : public UiTrackMouseElement {
public:
	NodeArea()
		: m_nodeRect({
			{ 0, 0, 200, 100 },
			{ 300, 150, 200, 100 },
		})
		, m_movingNode(-1)
	{}

	~NodeArea() {
		if (m_contextMenu) {
			delete m_contextMenu;
		}
	}

	void SetContextMenu(UiContextMenu * contextMenu) { m_contextMenu = contextMenu; }

public: // protected:
	void Paint(NVGcontext *vg) const override {
		UiTrackMouseElement::Paint(vg);
		const ::Rect & r = InnerRect();
		
		// Background
		nvgBeginPath(vg);
		nvgRect(vg, r.x, r.y, r.w, r.h);
		nvgFillColor(vg, nvgRGB(30, 57, 91));
		nvgFill(vg);
		
		// Nodes
		for (const ::Rect & nr : m_nodeRect) {
			nvgBeginPath(vg);
			nvgRect(vg, nr.x, nr.y, nr.w, nr.h);
			nvgFillColor(vg, nvgRGB(128, 57, 91));
			nvgFill(vg);

			nvgBeginPath(vg);
			nvgRect(vg, nr.x + .5f, nr.y + .5f, nr.w - 1, nr.h - 1);
			nvgStrokeColor(vg, nvgRGB(56, 57, 58));
			nvgStroke(vg);
		}
	}
	
	void OnMouseOver(int x, int y) override {
		UiTrackMouseElement::OnMouseOver(x, y);

		if (m_movingNode > -1 && m_movingNode < m_nodeRect.size()) {
			::Rect & nr = m_nodeRect[m_movingNode];
			nr.x = m_moveStartNodeX + MouseX() - m_moveStartMouseX;
			nr.y = m_moveStartNodeY + MouseY() - m_moveStartMouseY;
		}
	}

	void OnMouseClick(int button, int action, int mods) override {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			const ::Rect & r = InnerRect();

			// TODO: use tree
			for (int i = m_nodeRect.size() - 1; i >= 0; --i) {
				const ::Rect & nr = m_nodeRect[i];
				if (nr.Contains(MouseX(), MouseY())) {
					m_moveStartNodeX = nr.x;
					m_moveStartNodeY = nr.y;
					m_moveStartMouseX = MouseX();
					m_moveStartMouseY = MouseY();
					m_movingNode = i;
					break;
				}
			}
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			m_movingNode = -1;
		}

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			if (m_contextMenu) {
				m_contextMenu->Popup(MouseX(), MouseY());
			}
		}
	}

private:
	UiContextMenu * m_contextMenu;

	std::vector<::Rect> m_nodeRect;

	float m_nodeX;
	float m_nodeY;

	int m_moveStartMouseX;
	int m_moveStartMouseY;
	int m_moveStartNodeX;
	int m_moveStartNodeY;
	int m_movingNode;
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

	UiContextMenu *contextMenu = new UiContextMenu(popupLayout);
	contextMenu->SetItemLabels({ "Lorem", "Ipsum", "Dolor", "Sit", "Amet" });
	nodeArea->SetContextMenu(contextMenu);

	// Sidebar
	UiVBoxLayout *layout = new UiVBoxLayout();

	{
		Parameter *param = new Parameter();
		param->setType(StringType);
		param->setName("Yo");
		param->set(QString::fromStdString("bloum"));
		ParameterWidget *paramWidget = new ParameterWidget(popupLayout);
		paramWidget->SetParameter(param);
		paramWidget->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramWidget);
	}

	{
		Parameter *param = new Parameter();
		param->setType(IntType);
		param->setName("An int param");
		param->set(28);
		ParameterWidget *paramWidget = new ParameterWidget(popupLayout);
		paramWidget->SetParameter(param);
		paramWidget->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramWidget);
	}

	{
		Parameter *param = new Parameter();
		param->setType(EnumType);
		param->setName("Enum");
		param->insertMenuItems(0, 2);
		param->setMenuLabel(0, "Choice A");
		param->setMenuLabel(1, "Choice B");
		param->setMenuLabel(2, "Choice C");
		param->set(1);
		ParameterWidget *paramWidget = new ParameterWidget(popupLayout);
		paramWidget->SetParameter(param);
		paramWidget->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramWidget);
	}

	{
		Parameter *param = new Parameter();
		param->setType(ButtonType);
		param->setName("Push me!");
		ParameterWidget *paramWidget = new ParameterWidget(popupLayout);
		paramWidget->SetParameter(param);
		paramWidget->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramWidget);
	}

	UiButton *button = new UiButton();
	button->SetText("Test 1");
	button->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(button);

	button = new UiButton();
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
	enumInput->SetItemLabels({"Choice A", "Choice B"});
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
