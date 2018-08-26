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

enum NodeAreaItemType {
	_BeginNodeAreaItems,
	NodeItemType,
	SlotItemType,
	_EndNodeAreaItems,
};

/**
 * Add parent/child mechanism to QuadTree::Item
 */
class AbstractNodeAreaItem : public QuadTree::Item {
public:
	static AbstractNodeAreaItem * fromRawItem(QuadTree::Item *rawItem) {
		if (rawItem
			&& rawItem->Type() > _BeginNodeAreaItems
			&& rawItem->Type() < _EndNodeAreaItems) {
			return static_cast<AbstractNodeAreaItem*>(rawItem);
		}
		else {
			return nullptr;
		}
	}

public:
	AbstractNodeAreaItem(Rect bbox, int type)
		: QuadTree::Item(bbox, type)
	{}

	void AddChild(AbstractNodeAreaItem *child) { m_children.push_back(child); }
	const std::vector<AbstractNodeAreaItem*> & Children() const { return m_children; }

private:
	std::vector<AbstractNodeAreaItem*> m_children;
};

class NodeItem : public AbstractNodeAreaItem {
	/*
public:
	static NodeItem * fromRawItem(QuadTree::Item *rawItem) {
		if (rawItem && rawItem->Type() == NodeItemType) {
			return static_cast<NodeItem*>(rawItem);
		}
		else {
			return nullptr;
		}
	}
	*/

public:
	NodeItem(Rect bbox)
		: AbstractNodeAreaItem(bbox, NodeItemType)
	{}
};

class SlotItem : public AbstractNodeAreaItem {
public:
	SlotItem(Rect bbox)
		: AbstractNodeAreaItem(bbox, SlotItemType)
	{}

	virtual bool Hit(float x, float y) override {
		// treated as an ellipse
		const Rect & r = BBox();
		float hw = r.wf() / 2.f;
		float hh = r.hf() / 2.f;
		float cx = r.xf() + hw;
		float cy = r.yf() + hh;
		float dx = (x - cx) / hw;
		float dy = (y - cy) / hh;
		return (dx * dx + dy * dy) < 1.f;
	}
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
		: m_treeItems({
			new NodeItem({ 0, 0, 200, 100 }),
			new NodeItem({ 300, 150, 200, 100 }),
			new NodeItem({ 400, 200, 200, 100 }),
			new SlotItem({ 190, 30, 20, 20 }),
			new SlotItem({ 190, 60, 20, 20 }),
		})
		, m_tree(new QuadTree(250, 300, 500, 500, 5))
	{
		
		for (AbstractNodeAreaItem *item : m_treeItems) {
			m_tree->Insert(item);
		}

		m_treeItems[0]->AddChild(m_treeItems[3]); // DEBUG
		m_treeItems[0]->AddChild(m_treeItems[4]); // DEBUG
	}

	~NodeArea() {
		if (m_contextMenu) {
			delete m_contextMenu;
		}
		assert(m_tree);
		delete m_tree;
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
		for (AbstractNodeAreaItem *item : m_treeItems) {
			assert(item);
			const ::Rect & nr = item->BBox();

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

		for (MovingItem & movingNode : m_movingNodes) {
			AbstractNodeAreaItem *nodeItem = AbstractNodeAreaItem::fromRawItem(movingNode.acc.item);
			assert(nodeItem);
			::Rect & bbox = nodeItem->BBox();
			bbox.x = movingNode.startX + deltaX;
			bbox.y = movingNode.startY + deltaY;
			movingNode.acc = m_tree->UpdateItemBBox(movingNode.acc, bbox);
			// TODO: handle this better
			if (!movingNode.acc.IsValid()) {
				WARN_LOG << "Lost item; out of quadtree bounds";
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
				switch (acc.item->Type()) {
				case NodeItemType:
					m_selectedNodes.push_back(acc);
					StartMovingNodes();
					break;

				case SlotItemType:
					DEBUG_LOG << "Start dragging link";
				}
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
	void StartMovingNodes() {
		for (const QuadTree::Accessor & acc : m_selectedNodes) {
			AbstractNodeAreaItem *nodeItem = AbstractNodeAreaItem::fromRawItem(acc.item);
			if (!nodeItem) {
				continue;
			}

			const ::Rect & nr = nodeItem->BBox();
			m_movingNodes.push_back(MovingItem(acc, nr.x, nr.y));

			for (AbstractNodeAreaItem *child : nodeItem->Children()) {
				QuadTree::Accessor acc = m_tree->Find(child);
				if (acc.isValid) {
					const ::Rect & bbox = child->BBox();
					m_movingNodes.push_back(MovingItem(acc, bbox.x, bbox.y));
				}
			}
		}
		m_moveStartMouseX = MouseX();
		m_moveStartMouseY = MouseY();
	}

	void ClearSelection() {
		m_selectedNodes.clear();
	}

	void ClearMovingItems() {
		m_movingNodes.clear();
	}

private:
	UiContextMenu * m_contextMenu;

	// TODO: keep only references to nodeItems
	QuadTree *m_tree;
	std::vector<AbstractNodeAreaItem*> m_treeItems;

	int m_moveStartMouseX;
	int m_moveStartMouseY;
	std::vector<MovingItem> m_movingNodes;

	std::vector<QuadTree::Accessor> m_selectedNodes;
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
