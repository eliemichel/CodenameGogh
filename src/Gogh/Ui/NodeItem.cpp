#include "NodeItem.h"
#include "Node.h"

NodeItem::NodeItem(::Node *node, QuadTree *tree, UiLayout *popupLayout)
	: AbstractNodeAreaItem({ 0, 0, 200, 100 }, NodeItemType)
	, m_node(node)
	, m_content(nullptr)
	, m_selected(false)
{
	// Content
	SetContent(node->createDelegate(popupLayout));

	for (int i = 0; i < node->inputSlotCount(); ++i) {
		InputSlot & slot = node->inputSlot(i);
		SlotItem *slotItem = new SlotItem({ -8, 20 + 30 * i, 16, 16 });
		AddChild(slotItem);
	}
	for (int i = 0; i < node->outputSlotCount(); ++i) {
		OutputSlot & slot = node->outputSlot(i);
		SlotItem *slotItem = new SlotItem({ 192, 20 + 30 * i, 16, 16 });
		AddChild(slotItem);
	}

	if (tree) {
		tree->Insert(this);
	}

	// TODO: connect nodes
	node->destroyed.connect(this, &NodeItem::OnNodeDestroyed);
}

NodeItem::NodeItem(Rect bbox)
	: AbstractNodeAreaItem(bbox, NodeItemType)
	, m_content(nullptr)
	, m_selected(false)
{}

NodeItem::~NodeItem() {
	if (m_content) {
		delete m_content;
	}
}

void NodeItem::SetContent(UiElement *element) {
	m_content = element;
	UpdateGeometry();
}

void NodeItem::Paint(NVGcontext *vg) const {
	const Rect & r = BBox();

	nvgBeginPath(vg);
	nvgRect(vg, r.xf(), r.yf(), r.wf(), r.hf());
	nvgFillColor(vg, nvgRGB(128, 57, 91));
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgRect(vg, r.xf() + .5f, r.yf() + .5f, r.wf() - 1.f, r.hf() - 1.f);
	nvgStrokeColor(vg, IsSelected() ? nvgRGB(156, 157, 158) : nvgRGB(56, 57, 58));
	nvgStroke(vg);

	if (m_content) {
		m_content->Paint(vg);
	}

	AbstractNodeAreaItem::Paint(vg);
}

void NodeItem::UpdateGeometry() {
	if (m_content) {
		m_content->SetRect(BBox());
	}
}

void NodeItem::OnNodeDestroyed() {
	m_node = nullptr;
}
