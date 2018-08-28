#ifndef H_NODEITEM
#define H_NODEITEM

#include "NodeAreaItems.h"
class Node;

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
	NodeItem(Node *node, QuadTree *tree = nullptr, UiLayout *popupLayout = nullptr);
	NodeItem(Rect bbox);
	~NodeItem();

	void SetContent(UiElement *element);

	void Paint(NVGcontext *vg) const override;

protected:
	void UpdateGeometry() override;

private:
	UiElement * m_content;
};


#endif // H_NODEITEM
