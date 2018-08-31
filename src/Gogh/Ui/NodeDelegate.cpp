#include "NodeDelegate.h"
#include "Node.h"
#include "Parameter.h"
#include "ParameterDelegate.h"

#include <list>

NodeDelegate::NodeDelegate(Node *node, UiLayout *popupLayout)
	: m_node(node)
	, m_popupLayout(popupLayout)
{
	InsertParams(0, m_node->paramCount() - 1);

	// TODO: connect signals:
	node->insertedParams.connect(this, &NodeDelegate::InsertParams);
	node->aboutToRemoveParams.connect(this, &NodeDelegate::RemoveParams);
	/*
	void aboutToInsertParams(int first, int last);
	void aboutToRemoveParams(int first, int last);
	void aboutToInsertInputSlots(int first, int last);
	void aboutToRemoveInputSlots(int first, int last);
	void aboutToInsertOutputSlots(int first, int last);
	void aboutToRemoveOutputSlots(int first, int last);
	*/
}

void NodeDelegate::InsertParams(int first, int last) {
	// unstack items after first
	// TODO: add an InsertItem method in UiLayout
	std::list<UiElement*> m_lastElements;
	while (Items().size() > first) {
		m_lastElements.push_back(RemoveItem());
	}

	for (int i = first; i <= last; ++i) {
		ParameterDelegate *paramDelegate = new ParameterDelegate(m_popupLayout);
		paramDelegate->SetParameter(&m_node->param(i));
		paramDelegate->SetInnerSizeHint(0, 0, 0, 30);
		AddItem(paramDelegate);
	}

	// restack items
	while (!m_lastElements.empty()) {
		AddItem(m_lastElements.back());
		m_lastElements.pop_back();
	}

	RequestUpdate();
}

void NodeDelegate::RemoveParams(int first, int last) {
	// unstack items after first
	// TODO: add an RemoveItems range method in UiLayout
	std::list<UiElement*> m_lastElements;
	while (Items().size() > first) {
		if (Items().size() > last + 1) {
			m_lastElements.push_back(RemoveItem());
		} else {
			delete RemoveItem();
		}
	}

	// restack items
	while (!m_lastElements.empty()) {
		AddItem(m_lastElements.back());
		m_lastElements.pop_back();
	}

	RequestUpdate();
}
