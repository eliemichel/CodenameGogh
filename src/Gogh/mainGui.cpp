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

#include <algorithm>
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

	virtual void Paint(NVGcontext *vg) const {
		for (const AbstractNodeAreaItem* child : Children()) {
			child->Paint(vg);
		}
	}

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

	void Paint(NVGcontext *vg) const override {
		const Rect & r = BBox();

		nvgBeginPath(vg);
		nvgRect(vg, r.xf(), r.yf(), r.wf(), r.hf());
		nvgFillColor(vg, nvgRGB(128, 57, 91));
		nvgFill(vg);

		nvgBeginPath(vg);
		nvgRect(vg, r.xf() + .5f, r.yf() + .5f, r.wf() - 1.f, r.hf() - 1.f);
		nvgStrokeColor(vg, nvgRGB(56, 57, 58));
		nvgStroke(vg);

		AbstractNodeAreaItem::Paint(vg);
	}
};

class SlotItem : public AbstractNodeAreaItem {
public:
	SlotItem(Rect bbox)
		: AbstractNodeAreaItem(bbox, SlotItemType)
	{}

	bool Hit(float x, float y) override {
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

	void Paint(NVGcontext *vg) const override {
		const Rect & r = BBox();
		float hw = r.wf() / 2.f;
		float hh = r.hf() / 2.f;
		float cx = r.xf() + hw;
		float cy = r.yf() + hh;

		nvgBeginPath(vg);
		nvgEllipse(vg, cx, cy, hw, hh);
		nvgFillColor(vg, nvgRGB(255, 255, 255));
		nvgFill(vg);

		AbstractNodeAreaItem::Paint(vg);
	}
};

struct LinkItem {
	SlotItem *origin, *destination;
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
		: m_nodeItems({
			new NodeItem({ 0, 0, 200, 100 }),
			new NodeItem({ 300, 150, 200, 100 }),
			new NodeItem({ 400, 200, 200, 100 }),
		})
		, m_tree(new QuadTree(250, 300, 500, 500, 5))
	{
		
		for (NodeItem *item : m_nodeItems) {
			m_tree->Insert(item);
		}

		SlotItem *slot;

		slot = new SlotItem({ 192, 30, 16, 16 });
		m_tree->Insert(slot);
		m_nodeItems[0]->AddChild(slot); // DEBUG

		slot = new SlotItem({ 192, 60, 16, 16 });
		m_tree->Insert(slot);
		m_nodeItems[0]->AddChild(slot); // DEBUG
		SlotItem *slot1 = slot;

		slot = new SlotItem({ 292, 210, 16, 16 });
		m_tree->Insert(slot);
		m_nodeItems[1]->AddChild(slot); // DEBUG

		SortItems();

		m_linkItems.push_back({ slot1, slot });
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
		for (const AbstractNodeAreaItem *item : m_nodeItems) {
			item->Paint(vg);
		}

		// Links
		for (const LinkItem & link : m_linkItems) {
			const ::Rect & or = link.origin->BBox();
			const ::Rect & dr = link.destination->BBox();
			float ox = or.xf() + or.wf() / 2.f;
			float oy = or.yf() + or.hf() / 2.f;
			float dx = dr.xf() + dr.wf() / 2.f;
			float dy = dr.yf() + dr.hf() / 2.f;
			nvgBeginPath(vg);
			nvgMoveTo(vg, ox, oy);
			nvgLineTo(vg, dx, dy);
			nvgStrokeColor(vg, nvgRGB(255, 255, 255));
			nvgStroke(vg);
		}

		// DEBUG TREE
		if (m_debug) {
			m_tree->PaintDebug(vg);
		}
	}
	
	void OnMouseOver(int x, int y) override {
		UiTrackMouseElement::OnMouseOver(x, y);

		int deltaX = MouseX() - m_moveStartMouseX;
		int deltaY = MouseY() - m_moveStartMouseY;

		for (MovingItem & movingNode : m_movingNodes) {
			AbstractNodeAreaItem *nodeItem = AbstractNodeAreaItem::fromRawItem(movingNode.acc.item);
			assert(nodeItem);
			::Rect & bbox = nodeItem->BBox();
			::Rect oldBbox = bbox;
			bbox.x = movingNode.startX + deltaX;
			bbox.y = movingNode.startY + deltaY;
			movingNode.acc = m_tree->UpdateItemBBox(movingNode.acc, bbox);
			// TODO: handle this better
			if (!movingNode.acc.IsValid()) {
				WARN_LOG << "Lost item; out of quadtree bounds";
				m_tree->UpdateItemBBox(movingNode.acc, oldBbox);
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
					BringToTop(AbstractNodeAreaItem::fromRawItem(acc.item));
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

		// DEBUG
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
			m_debug = !m_debug;
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

	// Update items layer (a.k.a. depth) to avoid overflows and keep slots
	// in front of their parent nodes while still being close enough to it
	// TODO: avoid the complexity of O(n)
	void SortItems() {
		// sort nodes by their current layer
		std::sort(m_nodeItems.begin(), m_nodeItems.end(), [](NodeItem *left, NodeItem *right) { return left->Layer() < right->Layer(); });

		// normalize layer
		float layer = 0.f;
		for (NodeItem * nodeItem : m_nodeItems) {
			nodeItem->SetLayer(layer);
			// Place children on top
			for (AbstractNodeAreaItem *child : nodeItem->Children()) {
				child->SetLayer(layer + .5f);
			}
			layer += 1.f;
		}
	}

	// target can be null
	void BringToTop(AbstractNodeAreaItem *item) {
		float topMost = static_cast<float>(m_nodeItems.size() + 1);
		for (auto it = m_nodeItems.rbegin(); it != m_nodeItems.rend(); ++it) {
			// TODO: need IsSelected()
			/*
			if (!it->IsSelected()) {
				topMost = it->Layer() + .5f;
				break;
			}
			*/
		}
		item->SetLayer(topMost);
		SortItems();
	}

	void ClearSelection() {
		m_selectedNodes.clear();
	}

	void ClearMovingItems() {
		m_movingNodes.clear();
	}

private:
	UiContextMenu * m_contextMenu;

	bool m_debug;

	QuadTree *m_tree;
	std::vector<NodeItem*> m_nodeItems;

	int m_moveStartMouseX;
	int m_moveStartMouseY;
	std::vector<MovingItem> m_movingNodes;

	std::vector<QuadTree::Accessor> m_selectedNodes;

	std::vector<LinkItem> m_linkItems;
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
