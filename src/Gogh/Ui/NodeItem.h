#ifndef H_NODEITEM
#define H_NODEITEM

#include "NodeAreaItems.h"
class Node;
class InputSlotItem;
class OutputSlotItem;

class NodeItem : public AbstractNodeAreaItem {
public:
	static NodeItem * fromRawItem(QuadTree::Item *rawItem) {
		if (rawItem && rawItem->Type() == NodeItemType) {
			return static_cast<NodeItem*>(rawItem);
		} else {
			return nullptr;
		}
	}

public:
	NodeItem(::Node *node, QuadTree *tree = nullptr, UiLayout *popupLayout = nullptr);
	~NodeItem();

	::Node * Node() const { return m_node; }

	UiElement * Content() const { return m_content; }
	void SetContent(UiElement *element);

	bool IsSelected() const { return m_selected; }
	void SetSelected(bool selected = true) { m_selected = selected; }

	void Paint(NVGcontext *vg) const override;

protected:
	void UpdateGeometry() override;

private:
	void OnNodeDestroyed();
	void OnInsertedInputSlots(int first, int last);
	void WhenAboutToRemoveInputSlots(int first, int last);
	void OnInsertedOutputSlots(int first, int last);
	void WhenAboutToRemoveOutputSlots(int first, int last);

private:
	::Node * m_node;
	UiElement * m_content;
	std::vector<InputSlotItem*> m_inputSlotItems;
	std::vector<OutputSlotItem*> m_outputSlotItems;
	bool m_selected;
};


#endif // H_NODEITEM
