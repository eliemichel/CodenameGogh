#ifndef H_NODEAREA
#define H_NODEAREA

#include "UiBase.h"
#include "QuadTree.h"

class AbstractNodeAreaItem;
class NodeItem;
class SlotItem;
class UiContextMenu;

struct LinkItem {
	SlotItem *origin, *destination;
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
	NodeArea(UiLayout *popupLayout = nullptr);
	~NodeArea();

	void SetContextMenu(UiContextMenu * contextMenu) { m_contextMenu = contextMenu; }

public: // protected:
	void Paint(NVGcontext *vg) const override;
	void OnMouseOver(int x, int y) override;
	void OnMouseClick(int button, int action, int mods) override;
	void OnKey(int key, int scancode, int action, int mods) override;

private:
	void StartMovingNodes();

	// Update items layer (a.k.a. depth) to avoid overflows and keep slots
	// in front of their parent nodes while still being close enough to it
	// TODO: avoid the complexity of O(n)
	void SortItems();

	// target can be null
	void BringToTop(AbstractNodeAreaItem *item);

	void ClearSelection();

	void ClearMovingItems();

private:
	UiContextMenu * m_contextMenu;

	bool m_debug;

	QuadTree *m_tree;
	std::vector<NodeItem*> m_nodeItems;

	int m_moveStartMouseX;
	int m_moveStartMouseY;
	std::vector<MovingItem> m_movingNodes;

	std::vector<QuadTree::Accessor> m_selectedNodes;

	std::vector<LinkItem> m_linkItems;
};


#endif // H_NODEAREA
