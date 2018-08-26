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
		, m_topRight(nullptr)
		, m_topLeft(nullptr)
		, m_bottomRight(nullptr)
		, m_bottomLeft(nullptr)
	{}

	~QuadTree() {
		if (!IsLeaf()) {
			delete m_topRight;
			delete m_topLeft;
			delete m_bottomRight;
			delete m_bottomLeft;
		}
	}

	bool IsLeaf() const { return m_topLeft == nullptr; }

	bool IsEmpty() const { return IsLeaf() && m_items.empty(); }

	void Insert(Item item) {
		if (QuadTree *child = FitInChild(item)) {
			child->Insert(item);
		} else {
			m_items.push_back(item);
			if (m_items.size() > 1) {
				Split();
			}
		}
	}

	// Remove the topmost element located at target pos
	Item PopAt(float x, float y) {
		for (auto it = m_items.begin(); it != m_items.end();) {
			if (it->bbox.Contains(x, y)) {
				Item item = *it;
				it = m_items.erase(it);
				Prune(true /* recursive */);
				return item;
			}
			else {
				++it;
			}
		}

		if (!IsLeaf()) {
			Item item;
			if (x < m_cx) {
				if (y < m_cy) {
					item = m_topLeft->PopAt(x, y);
				} else {
					item = m_bottomLeft->PopAt(x, y);
				}
			} else {
				if (y < m_cy) {
					item = m_topRight->PopAt(x, y);
				} else {
					item = m_bottomRight->PopAt(x, y);
				}
			}
			Prune(false /* recursive */);
			return item;
		}

		return Item();
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
			m_topRight->PaintDebug(vg);
			m_topLeft->PaintDebug(vg);
			m_bottomRight->PaintDebug(vg);
			m_bottomLeft->PaintDebug(vg);
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
		m_topRight = new QuadTree(m_cx + qw, m_cy - qh, qw, qh, m_divisions - 1);
		m_topLeft = new QuadTree(m_cx - qw, m_cy - qh, qw, qh, m_divisions - 1);
		m_bottomRight = new QuadTree(m_cx + qw, m_cy + qh, qw, qh, m_divisions - 1);
		m_bottomLeft = new QuadTree(m_cx - qw, m_cy + qh, qw, qh, m_divisions - 1);

		for (auto it = m_items.begin(); it != m_items.end();) {
			if (QuadTree *child = FitInChild(*it)) {
				child->Insert(*it);
				it = m_items.erase(it);
			} else {
				++it;
			}
		}
	}

	/// Prune unused children recursively
	/// Return the number of elements in the tree
	/// Populate item if it is the only one in the tree, and pop it before, so
	/// you'd have to insert it back.
	int Prune(bool recursive) {
		if (IsLeaf()) {
			return m_items.size();
		} else {
			int tln;
			int trn;
			int bln;
			int brn;
			if (recursive) {
				tln = m_topLeft->Prune(recursive);
				trn = m_topRight->Prune(recursive);
				bln = m_bottomLeft->Prune(recursive);
				brn = m_bottomRight->Prune(recursive);
			} else {
				tln = m_topLeft->m_items.size();
				trn = m_topRight->m_items.size();
				bln = m_bottomLeft->m_items.size();
				brn = m_bottomRight->m_items.size();
			}
			int sum = m_items.size() + tln + trn + bln + brn;

			if (sum == 1) {

				assert(m_topLeft->IsLeaf());
				assert(m_topRight->IsLeaf());
				assert(m_bottomLeft->IsLeaf());
				assert(m_bottomRight->IsLeaf());


				bool prune = true;
				if (m_topLeft->IsLeaf() && tln == 1) {
					m_items.push_back(m_topLeft->m_items.back());
					m_topLeft->m_items.pop_back();
				} else if (m_topRight->IsLeaf() && trn == 1) {
					m_items.push_back(m_topRight->m_items.back());
					m_topRight->m_items.pop_back();
				} else if(m_bottomLeft->IsLeaf() && bln == 1) {
					m_items.push_back(m_bottomLeft->m_items.back());
					m_bottomLeft->m_items.pop_back();
				} else if (m_bottomRight->IsLeaf() && brn == 1) {
					m_items.push_back(m_bottomRight->m_items.back());
					m_bottomRight->m_items.pop_back();
				} else if (m_items.size()) {
					// noop
				} else {
					prune = false;
				}

				assert(prune);

				if (prune) {
					delete m_topRight;
					delete m_topLeft;
					delete m_bottomRight;
					delete m_bottomLeft;
					m_topRight = nullptr;
					m_topLeft = nullptr;
					m_bottomRight = nullptr;
					m_bottomLeft = nullptr;
				}
			}
		}
	}

	/// If the item fits in one of the children, return a pointer to this
	/// child, otherwise return nullptr.
	QuadTree * FitInChild(Item item) {
		if (IsLeaf()) {
			return nullptr;
		}

		bool fullLeft = item.bbox.x + item.bbox.w <= m_cx;
		bool fullRight = item.bbox.x >= m_cx;
		bool fullTop = item.bbox.y + item.bbox.h <= m_cy;
		bool fullBottom = item.bbox.y >= m_cy;

		if (fullLeft && fullTop) {
			return m_topLeft;
		} else if (fullLeft && fullBottom) {
			return m_bottomLeft;
		} else if (fullRight && fullTop) {
			return m_topRight;
		} else if (fullRight && fullBottom) {
			return m_bottomRight;
		} else {
			return nullptr;
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
	QuadTree *m_topRight, *m_topLeft, *m_bottomRight, *m_bottomLeft;
	/// Items that cannot fit in children, whether it is because there is no
	/// child tree or because the item's bbox is too large.
	std::vector<Item> m_items;
};

class NodeArea : public UiTrackMouseElement {
public:
	NodeArea()
		: m_nodeRect({
			{ 0, 0, 200, 100 },
			{ 300, 150, 200, 100 },
			{ 400, 200, 200, 100 },
		})
		, m_movingNode(-1)
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
			/*
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
			*/

			QuadTree::Item item = m_tree->PopAt(MouseX(), MouseY());
			if (item.isValid) {
				const ::Rect & nr = m_nodeRect[item.index];
				m_moveStartNodeX = nr.x;
				m_moveStartNodeY = nr.y;
				m_moveStartMouseX = MouseX();
				m_moveStartMouseY = MouseY();
				m_movingNode = item.index;
			}
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			if (m_movingNode > -1 && m_movingNode < m_nodeRect.size()) {
				m_tree->Insert(QuadTree::Item(m_nodeRect[m_movingNode], m_movingNode));
			}
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

	int m_moveStartMouseX;
	int m_moveStartMouseY;
	int m_moveStartNodeX;
	int m_moveStartNodeY;
	int m_movingNode;

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
