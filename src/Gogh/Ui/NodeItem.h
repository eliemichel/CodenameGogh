#ifndef H_NODEITEM
#define H_NODEITEM

#include "NodeAreaItems.h"
class Node;

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
	NodeItem(Node *node, QuadTree *tree = nullptr, UiLayout *popupLayout = nullptr);
	NodeItem(Rect bbox);
	~NodeItem();

	UiElement * Content() const { return m_content; }
	void SetContent(UiElement *element);

	bool IsSelected() const { return m_selected; }
	void SetSelected(bool selected = true) { m_selected = selected; }

	void Paint(NVGcontext *vg) const override;

protected:
	void UpdateGeometry() override;

private:
	UiElement * m_content;
	bool m_selected;
};


#endif // H_NODEITEM
