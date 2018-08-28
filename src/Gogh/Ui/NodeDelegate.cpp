#include "NodeDelegate.h"
#include "Node.h"
#include "Parameter.h"
#include "ParameterDelegate.h"

NodeDelegate::NodeDelegate(Node *node, UiLayout *popupLayout)
	: m_node(node)
{
	for (int i = 0; i < m_node->paramCount(); ++i) {
		ParameterDelegate *paramDelegate = new ParameterDelegate(popupLayout);
		paramDelegate->SetParameter(&m_node->param(i));
		paramDelegate->SetInnerSizeHint(0, 0, 0, 30);
		AddItem(paramDelegate);
	}

	// TODO: connect signals:
	/*
	void aboutToInsertParams(int first, int last);
	void aboutToRemoveParams(int first, int last);
	void aboutToInsertInputSlots(int first, int last);
	void aboutToRemoveInputSlots(int first, int last);
	void aboutToInsertOutputSlots(int first, int last);
	void aboutToRemoveOutputSlots(int first, int last);
	*/
}
