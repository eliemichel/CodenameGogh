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
#include "Ui/QuadTree.h"
#include "Ui/UiContextMenu.h"

#include <GLFW/glfw3.h>
#include <nanovg.h>

#include <cassert>

class NodeAreaTreeItem : public QuadTree::Item {
public:
	enum Type {
		DefaultType,
	};

public:
	NodeAreaTreeItem(Rect bbox, int index)
		: QuadTree::Item(bbox, DefaultType)
		, m_index(index)
	{}

	int Index() const { return m_index; }

private:
	int m_index;
};

class NodeArea : public UiTrackMouseElement {
private:
	struct MovingItem {
		MovingItem(QuadTree::Accessor _acc, int _startX, int _startY) : acc(_acc), startX(_startX), startY(_startY) {}
		QuadTree::Accessor acc;
		int startX;
		int startY;
	};

public:
	NodeArea()
		: m_nodeRect({
			{ 0, 0, 200, 100 },
			{ 300, 150, 200, 100 },
			{ 400, 200, 200, 100 },
			{ 190, 100, 20, 20 },
		})
		, m_tree(nullptr)
	{
		m_tree = new QuadTree(250, 300, 500, 500, 5);
		for (int i = 0; i < m_nodeRect.size(); ++i) {
			m_tree->Insert(new NodeAreaTreeItem(m_nodeRect[i], i));
		}
	}

	~NodeArea() {
		if (m_contextMenu) {
			delete m_contextMenu;
		}
		if (m_tree) {
			delete m_tree;
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

		// DEBUG TREE
		m_tree->PaintDebug(vg);
	}
	
	void OnMouseOver(int x, int y) override {
		UiTrackMouseElement::OnMouseOver(x, y);

		int deltaX = MouseX() - m_moveStartMouseX;
		int deltaY = MouseY() - m_moveStartMouseY;
		for (MovingItem & item : m_movingItems) {
			assert(item.acc.item->Type() == NodeAreaTreeItem::DefaultType);
			int i = static_cast<NodeAreaTreeItem*>(item.acc.item)->Index();
			if (i > -1 && i < m_nodeRect.size()) {
				::Rect & nr = m_nodeRect[i];
				nr.x = item.startX + deltaX;
				nr.y = item.startY + deltaY;
				item.acc = m_tree->UpdateItemBBox(item.acc, nr);
			}
		}
	}

	void OnMouseClick(int button, int action, int mods) override {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			const ::Rect & r = InnerRect();

			if (!(mods & GLFW_MOD_SHIFT)) {
				ClearSelection();
			}

			QuadTree::Accessor acc = m_tree->ItemAt(MouseX(), MouseY());
			if (acc.isValid) {
				m_selectedItems.push_back(acc);
			}
			
			for (const QuadTree::Accessor & acc : m_selectedItems) {
				assert(acc.item->Type() == NodeAreaTreeItem::DefaultType);
				int i = static_cast<NodeAreaTreeItem*>(acc.item)->Index();
				const ::Rect & nr = m_nodeRect[i];
				m_movingItems.push_back(MovingItem(acc, nr.x, nr.y));
			}
			m_moveStartMouseX = MouseX();
			m_moveStartMouseY = MouseY();

			// DEBUG
			if (mods & GLFW_MOD_CONTROL) {
				m_tree->RemoveItems(m_selectedItems);
			}
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			ClearMovingItems();
		}

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			if (m_contextMenu) {
				m_contextMenu->Popup(MouseX(), MouseY());
			}
		}
	}

	private:
		void ClearSelection() {
			m_selectedItems.clear();
		}

		void ClearMovingItems() {
			m_movingItems.clear();
		}

private:
	UiContextMenu * m_contextMenu;

	std::vector<::Rect> m_nodeRect;

	int m_moveStartMouseX;
	int m_moveStartMouseY;
	std::vector<MovingItem> m_movingItems;

	std::vector<QuadTree::Accessor> m_selectedItems;

	QuadTree *m_tree;
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
