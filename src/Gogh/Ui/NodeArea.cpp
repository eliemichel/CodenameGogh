#include "NodeArea.h"
#include "QuadTree.h"
#include "NodeAreaItems.h"
#include "NodeItem.h"
#include "UiContextMenu.h"
#include "Logger.h"
#include "Node.h"
#include "Parameter.h"
#include "Slot.h"
#include "Graph.h"
#include "UiButton.h"

#include <cassert>

NodeArea::MovingItem::MovingItem(QuadTree::Accessor _acc)
	: acc(_acc)
{
	if (_acc.item) {
		const ::Rect & bbox = acc.item->BBox();
		startX = bbox.x;
		startY = bbox.y;
	}
}

NodeArea::NodeArea(Graph *graph, UiLayout *popupLayout)
	: m_graph(graph)
	, m_contextMenu(nullptr)
	, m_nodeItems({
	new NodeItem({ 0, 0, 200, 100 }),
	new NodeItem({ 300, 150, 200, 100 }),
	new NodeItem({ 400, 200, 200, 100 }),
	})
	, m_tree(new QuadTree(250, 300, 500, 500, 5))
	, m_pendingLink({ nullptr, nullptr })
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

	m_linkItems.push_back({ slot1, slot });

	for (Node *node : graph->nodes()) {
		m_nodeItems.push_back(new NodeItem(node, m_tree, popupLayout));
	}

	SortItems();
}

NodeArea::~NodeArea() {
	if (m_contextMenu) {
		delete m_contextMenu;
	}
	assert(m_tree);
	delete m_tree;
}

void NodeArea::OnTick(float time) {
	// Clean up items that no longer represent a Node (because it has been destroyed)
	for (auto it = m_nodeItems.begin(); it != m_nodeItems.end();) {
		if (!(*it)->Node()) {
			it = m_nodeItems.erase(it);
		} else {
			++it;
		}
	}
}

void NodeArea::Paint(NVGcontext *vg) const {
	const ::Rect & r = InnerRect();

	// Background
	nvgBeginPath(vg);
	nvgRect(vg, r.xf(), r.yf(), r.wf(), r.hf());
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

	if (m_pendingLink.origin || m_pendingLink.destination) {
		const ::Rect & or = m_pendingLink.origin ? m_pendingLink.origin->BBox() : ::Rect(MouseX(), MouseY(), 0, 0);
		const ::Rect & dr = m_pendingLink.destination ? m_pendingLink.destination->BBox() : ::Rect(MouseX(), MouseY(), 0, 0);
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

void NodeArea::OnMouseOver(int x, int y) {
	UiTrackMouseLayout::OnMouseOver(x, y);

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

void NodeArea::OnMouseClick(int button, int action, int mods) {
	UiTrackMouseLayout::OnMouseClick(button, action, mods);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		const ::Rect & r = InnerRect();

		if (!(mods & GLFW_MOD_SHIFT)) {
			ClearSelection();
		}

		QuadTree::Accessor acc = m_tree->ItemAt(static_cast<float>(MouseX()), static_cast<float>(MouseY()));
		if (acc.isValid) {
			switch (acc.item->Type()) {
			case NodeItemType:
			{
				NodeItem *nodeItem = NodeItem::fromRawItem(acc.item);
				if (!nodeItem->IsSelected()) {
					SelectNode(nodeItem, acc);
					BringToTop(nodeItem);
				}
				StartMovingNodes();
				break;
			}

			case SlotItemType:
			{
				SlotItem *slotItem = SlotItem::fromRawItem(acc.item);
				m_pendingLink.origin = slotItem;
				m_pendingLink.destination = nullptr;
				DEBUG_LOG << "Start dragging link";
				break;
			}
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		ClearMovingItems();

		m_pendingLink.origin = m_pendingLink.destination = nullptr;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (m_contextMenu) {
			m_contextMenu->Popup(MouseX(), MouseY());
		}
	}
}

void NodeArea::OnKey(int key, int scancode, int action, int mods) {
	UiTrackMouseLayout::OnKey(key, scancode, action, mods);

	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
		std::vector<QuadTree::Accessor> accs;
		accs.reserve(m_selectedNodes.size());
		for (const SelectionEntry & entry : m_selectedNodes) {
			// Remove node item from m_nodeItems
			// TODO: avoid this O(n²) complexity, maybe use a set for node items, or mark items to be deleted
			/*
			auto it = std::find(m_nodeItems.begin(), m_nodeItems.end(), entry.nodeItem);
			if (it != m_nodeItems.end()) {
				m_nodeItems.erase(it);
			}
			*/
			delete entry.nodeItem->Node();
			accs.push_back(entry.acc);
		}
		m_tree->RemoveItems(accs);
		m_selectedNodes.clear();
	}

	// DEBUG
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		m_debug = !m_debug;
	}
}

UiElement * NodeArea::ItemAt(int x, int y) {
	QuadTree::Accessor acc = m_tree->ItemAt(static_cast<float>(MouseX()), static_cast<float>(MouseY()));
	if (!acc.isValid || acc.item->Type() != NodeItemType) {
		return nullptr;
	}
	
	NodeItem *nodeItem = NodeItem::fromRawItem(acc.item);
	return nodeItem->Content();
}

void NodeArea::SelectNode(NodeItem *nodeItem, const QuadTree::Accessor & acc) {
	nodeItem->SetSelected();
	m_selectedNodes.push_back({ nodeItem, acc });
}

void NodeArea::StartMovingNodes() {
	for (const SelectionEntry & entry : m_selectedNodes) {
		AbstractNodeAreaItem *nodeItem = AbstractNodeAreaItem::fromRawItem(entry.acc.item);
		if (!entry.nodeItem) {
			continue;
		}

		m_movingNodes.push_back(MovingItem(entry.acc));

		// Add children to moving nodes
		for (AbstractNodeAreaItem *child : entry.nodeItem->Children()) {
			QuadTree::Accessor acc = m_tree->Find(child);
			if (acc.isValid) {
				m_movingNodes.push_back(MovingItem(acc));
			}
		}
	}
	m_moveStartMouseX = MouseX();
	m_moveStartMouseY = MouseY();
}

void NodeArea::SortItems() {
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

void NodeArea::BringToTop(AbstractNodeAreaItem *item) {
	float topMost = static_cast<float>(m_nodeItems.size() + 1);
	for (std::vector<NodeItem*>::reverse_iterator it = m_nodeItems.rbegin(); it != m_nodeItems.rend(); ++it) {
		if (!(*it)->IsSelected()) {
			topMost = (*it)->Layer() + .5f;
			break;
		}
	}
	item->SetLayer(topMost);
	SortItems();
}

void NodeArea::ClearSelection() {
	for (const SelectionEntry & entry : m_selectedNodes) {
		entry.nodeItem->SetSelected(false);
	}
	m_selectedNodes.clear();
}

void NodeArea::ClearMovingItems() {
	m_movingNodes.clear();
}
