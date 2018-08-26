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

#include <cassert>
#include <list>
#include <limits>

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

class QuadTree {
public:
	enum Branch {
		NoBranch = -1,
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight,
		_BranchCount,
	};

	struct Accessor {
		std::list<Branch> path;
		union {
			void *data;
			int index;
		};
		bool isValid = false;
	};

	struct Item {
		Item() : isValid(false) {}
		Item(Rect _bbox, void *_data) : bbox(_bbox), data(_data), isValid(true) {}
		Item(Rect _bbox, int _index) : bbox(_bbox), index(_index), isValid(true) {}

		Rect bbox;
		union {
			void *data;
			int index;
		};
		bool isValid;
	};

public:
	QuadTree(float cx, float cy, float hw, float hh, int divisions)
		: m_cx(cx)
		, m_cy(cy)
		, m_hw(hw)
		, m_hh(hh)
		, m_divisions(divisions)
	{
		for (int i = 0; i < _BranchCount; ++i) {
			m_branches[i] = nullptr;
		}
	}

	~QuadTree() {
		if (!IsLeaf()) {
			for (int i = 0; i < _BranchCount; ++i) {
				delete m_branches[i];
			}
		}
	}

	bool IsLeaf() const { return m_branches[0] == nullptr; }

	/// Insert at the deepest possible node, branching while the division limit has not been reached
	Accessor Insert(Item item) {
		Branch branchIndex = FitInBranch(item);
		if (branchIndex != NoBranch) {
			if (IsLeaf()) {
				Split();
			}
			Accessor acc = m_branches[branchIndex]->Insert(item);
			acc.path.push_back(branchIndex);
			return acc;
		} else {
			m_items.push_back(item);
			Accessor acc;
			acc.isValid = true;
			acc.data = item.data;
			return acc;
		}
	}

	Accessor ItemAt(float x, float y) {
		Accessor candidate;
		int candidateLayer;

		for (const auto & item : m_items) {
			if (item.bbox.Contains(x, y)) {
				int itemLayer = 0; // TODO: manage layers
				if (!candidate.isValid || itemLayer > candidateLayer) {
					candidate.isValid = true;
					candidate.data = item.data;
					candidateLayer = itemLayer;
				}
			}
		}

		if (!IsLeaf()) {
			Branch branch = (
				x < m_cx
				? (y < m_cy ? TopLeft : BottomLeft)
				: (y < m_cy ? TopRight : BottomRight)
			);

			Accessor acc = m_branches[branch]->ItemAt(x, y);
			acc.path.push_back(branch);

			int itemLayer = 0; // TODO: manage layers
			if (acc.isValid && (!candidate.isValid || itemLayer > candidateLayer)) {
				candidate = acc;
				candidateLayer = itemLayer;
			}
		}

		return candidate;
	}

	/// Pop items matching the provided data
	void RemoveItems(const std::vector<Accessor> & accessors) {
		// split item lists
		std::vector<Accessor> subAccessors[_BranchCount];

		for (Accessor acc : accessors) {
			if (acc.path.empty()) {
				// search in items
				for (auto it = m_items.begin(); it != m_items.end();) {
					if (it->data == acc.data) {
						it = m_items.erase(it);
					} else {
						++it;
					}
				}
			} else {
				Branch branchIndex = acc.path.back();
				acc.path.pop_back();

				subAccessors[branchIndex].push_back(acc);
			}
		}

		if (!IsLeaf()) {
			for (int i = 0; i < _BranchCount; ++i) {
				m_branches[i]->RemoveItems(subAccessors[i]);
			}
		}

		Prune();
	}

	void RemoveItem(const Accessor & acc) {
		RemoveItems({ acc });
	}

	Accessor UpdateItemBBox(const Accessor & acc, Rect bbox) {
		Accessor newAcc = acc;
		newAcc.isValid = false;

		// update recursively
		if (!newAcc.path.empty()) {
			Branch branchIndex = newAcc.path.back();
			newAcc.path.pop_back();

			if (!IsLeaf()) {
				newAcc = m_branches[branchIndex]->UpdateItemBBox(newAcc, bbox);
				if (newAcc.isValid) {
					newAcc.path.push_back(branchIndex);
				}
			}
		}

		if (!newAcc.isValid) {
			bool fit = bbox.x >= m_cx - m_hw && bbox.x + bbox.w <= m_cx + m_hw
				&& bbox.y >= m_cy - m_hh && bbox.y + bbox.h <= m_cy + m_hh;
			
			// remove from items
			newAcc.isValid = true;
			RemoveItem(newAcc);

			// re-insert if possible, potentially inserting deeper
			if (fit) {
				newAcc = Insert(Item(bbox, newAcc.data));
			}

			// Flag invalid if new bbox does not fit for parent to re-insert the item
			newAcc.isValid = fit;
		}

		return newAcc;
	}

	void PaintDebug(NVGcontext *vg) const {
		nvgBeginPath(vg);
		nvgRect(vg, m_cx - m_hw, m_cy - m_hh, 2.f * m_hw, 2.f * m_hh);
		nvgFillColor(vg, nvgRGBA(0, 255, 0, 32));
		nvgFill(vg);

		nvgBeginPath(vg);
		nvgRect(vg, m_cx - m_hw + .5f, m_cy - m_hh + .5f, 2.f * m_hw - 1.f, 2.f * m_hh - 1.f);
		nvgStrokeColor(vg, nvgRGBA(0, 255, 0, 32));
		nvgStroke(vg);

		if (!IsLeaf()) {
			for (int i = 0; i < _BranchCount; ++i) {
				m_branches[i]->PaintDebug(vg);
			}
		}

		for (const Item & item : m_items) {
			nvgBeginPath(vg);
			nvgRect(vg, item.bbox.x, item.bbox.y, item.bbox.w, item.bbox.h);
			nvgFillColor(vg, IsLeaf() ? nvgRGBA(0, 0, 255, 32) : nvgRGBA(255, 0, 0, 32));
			nvgFill(vg);

			nvgBeginPath(vg);
			nvgRect(vg, item.bbox.x + .5f, item.bbox.y + .5f, item.bbox.w - 1.f, item.bbox.h - 1.f);
			nvgStrokeColor(vg, IsLeaf() ? nvgRGBA(0, 0, 255, 32) : nvgRGBA(255, 0, 0, 32));
			nvgStroke(vg);
		}
	}

private:
	void Split() {
		if (!IsLeaf() || m_divisions <= 0) {
			return;
		}

		float qw = m_hw / 2.f;
		float qh = m_hh / 2.f;
		m_branches[TopRight] = new QuadTree(m_cx + qw, m_cy - qh, qw, qh, m_divisions - 1);
		m_branches[TopLeft] = new QuadTree(m_cx - qw, m_cy - qh, qw, qh, m_divisions - 1);
		m_branches[BottomRight] = new QuadTree(m_cx + qw, m_cy + qh, qw, qh, m_divisions - 1);
		m_branches[BottomLeft] = new QuadTree(m_cx - qw, m_cy + qh, qw, qh, m_divisions - 1);
	}

	/// Prune unused children
	void Prune() {
		if (IsLeaf()) {
			return;
		}

		bool canPrune = true;
		for (int i = 0; i < _BranchCount; ++i) {
			canPrune = canPrune && m_branches[i]->IsLeaf() && m_branches[i]->m_items.empty();
		}

		if (canPrune) {
			for (int i = 0; i < _BranchCount; ++i) {
				delete m_branches[i];
				m_branches[i] = nullptr;
			}
		}
	}

	/// If the item fits in one of the children, return a pointer to this
	/// child, otherwise return nullptr.
	Branch FitInBranch(Item item) {
		if (m_divisions <= 0) {
			return NoBranch;
		}

		bool fullLeft = item.bbox.x + item.bbox.w <= m_cx;
		bool fullRight = item.bbox.x >= m_cx;
		bool fullTop = item.bbox.y + item.bbox.h <= m_cy;
		bool fullBottom = item.bbox.y >= m_cy;

		if (fullLeft && fullTop) {
			return TopLeft;
		} else if (fullLeft && fullBottom) {
			return BottomLeft;
		} else if (fullRight && fullTop) {
			return TopRight;
		} else if (fullRight && fullBottom) {
			return BottomRight;
		} else {
			return NoBranch;
		}
	}

private:
	/// Center
	float m_cx, m_cy;
	/// Half width and height
	float m_hw, m_hh;
	/// The maximum number of potential subdivisions, decreasing for children
	int m_divisions;

	/// Children, always all null or all non null
	QuadTree *m_branches[_BranchCount];
	/// Items that cannot fit in children, whether it is because there is no
	/// child tree or because the item's bbox is too large.
	std::vector<Item> m_items;
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
			m_tree->Insert(QuadTree::Item(m_nodeRect[i], i));
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
			if (item.acc.index > -1 && item.acc.index < m_nodeRect.size()) {
				::Rect & nr = m_nodeRect[item.acc.index];
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
				const ::Rect & nr = m_nodeRect[acc.index];
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
