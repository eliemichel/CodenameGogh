#include "NodeItem.h"
#include "SlotItem.h"
#include "Node.h"
#include "Logger.h"

NodeItem::NodeItem(::Node *node, QuadTree *tree, UiLayout *popupLayout)
	: AbstractNodeAreaItem({ 0, 0, 200, 150 }, NodeItemType)
	, m_node(node)
	, m_content(nullptr)
	, m_selected(false)
{
	// Content
	SetContent(node->createDelegate(popupLayout));

	OnInsertedInputSlots(0, node->inputSlotCount() - 1);
	OnInsertedOutputSlots(0, node->outputSlotCount() - 1);

	if (tree) {
		tree->Insert(this);
	}

	node->destroyed.connect(this, &NodeItem::OnNodeDestroyed);
	node->insertedInputSlots.connect(this, &NodeItem::OnInsertedInputSlots);
	node->aboutToRemoveInputSlots.connect(this, &NodeItem::WhenAboutToRemoveInputSlots);
	node->insertedOutputSlots.connect(this, &NodeItem::OnInsertedOutputSlots);
	node->aboutToRemoveOutputSlots.connect(this, &NodeItem::WhenAboutToRemoveOutputSlots);
}

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
	if (!m_node) {
		return;
	}

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

void NodeItem::OnInsertedInputSlots(int first, int last) {
	for (int i = first; i <= last; ++i) {
		Rect rect = BBox();
		InputSlot & slot = Node()->inputSlot(i);
		InputSlotItem *slotItem = new InputSlotItem(&slot, { rect.x - 8, rect.y + 20 + 30 * i, 16, 16 });
		AddChild(slotItem);
		m_inputSlotItems.push_back(slotItem);
	}

	// Shift the next slots
	for (int i = last + 1; i < Node()->inputSlotCount(); ++i) {
		Rect rect(-8, 20 + 30 * i, 16, 16);
		Tree()->UpdateItemBBox(Tree()->Find(m_inputSlotItems[i]), rect);
	}
}

void NodeItem::WhenAboutToRemoveInputSlots(int first, int last) {
	// Shift the next slots
	int count = last - first + 1;
	for (int i = last + 1; i < Node()->inputSlotCount(); ++i) {
		Rect rect(-8, 20 + 30 * (i - count), 16, 16);
		Tree()->UpdateItemBBox(Tree()->Find(m_inputSlotItems[i]), rect);
	}

	auto it = m_inputSlotItems.begin() + first;
	for (int i = first; i <= last; ++i) {
		InputSlotItem *slotItem = *it;
		RemoveChild(slotItem);
		delete slotItem;
		it = m_inputSlotItems.erase(it);
	}
}

void NodeItem::OnInsertedOutputSlots(int first, int last) {
	for (int i = first; i <= last; ++i) {
		Rect rect = BBox();
		OutputSlot & slot = Node()->outputSlot(i);
		OutputSlotItem *slotItem = new OutputSlotItem(&slot, { rect.x + 192, rect.y + 20 + 30 * i, 16, 16 });
		AddChild(slotItem);
		m_outputSlotItems.push_back(slotItem);
	}

	// Shift the next slots
	for (int i = last + 1; i < Node()->outputSlotCount(); ++i) {
		Rect rect(-8, 20 + 30 * i, 16, 16);
		Tree()->UpdateItemBBox(Tree()->Find(m_outputSlotItems[i]), rect);
	}
}

void NodeItem::WhenAboutToRemoveOutputSlots(int first, int last) {
	// Shift the next slots
	int count = last - first + 1;
	for (int i = last + 1; i < Node()->outputSlotCount(); ++i) {
		Rect rect(-8, 20 + 30 * (i - count), 16, 16);
		Tree()->UpdateItemBBox(Tree()->Find(m_outputSlotItems[i]), rect);
	}

	auto it = m_outputSlotItems.begin() + first;
	for (int i = first; i <= last; ++i) {
		OutputSlotItem *slotItem = *it;
		RemoveChild(slotItem);
		delete slotItem;
		it = m_outputSlotItems.erase(it);
	}
}
