#ifndef H_NODEAREA
#define H_NODEAREA

#include "UiBase.h"
#include "QuadTree.h"

class Graph;
class AbstractNodeAreaItem;
class NodeItem;
class InputSlotItem;
class OutputSlotItem;
class LinkItem;
class UiContextMenu;

class NodeArea : public UiTrackMouseLayout {
private:
	struct MovingItem {
		MovingItem(QuadTree::Accessor _acc);
		QuadTree::Accessor acc;
		int startX;
		int startY;
	};

	struct PendingLinkItem {
		OutputSlotItem *origin;
		InputSlotItem *destination;
	};

	// Node item and its accessor for quicker tree access
	struct SelectionEntry {
		NodeItem *nodeItem;
		QuadTree::Accessor acc;
	};

public:
	NodeArea(Graph *graph, UiLayout *popupLayout = nullptr);
	~NodeArea();

	void SetGraph(Graph *graph);
	void SetContextMenu(UiContextMenu * contextMenu) { m_contextMenu = contextMenu; }

public: // protected:
	void OnTick(float time) override;
	void Paint(NVGcontext *vg) const override;
	void OnMouseOver(int x, int y) override;
	void OnMouseClick(int button, int action, int mods) override;
	void OnKey(int key, int scancode, int action, int mods) override;

protected:
	UiElement * ItemAt(int x, int y) override;

private:
	void SelectNode(NodeItem *nodeItem, const QuadTree::Accessor & acc);
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
	UiLayout *m_popupLayout;
	Graph * m_graph;
	UiContextMenu * m_contextMenu;

	bool m_debug;

	QuadTree *m_tree;
	std::vector<NodeItem*> m_nodeItems;
	std::vector<LinkItem*> m_linkItems;

	int m_moveStartMouseX;
	int m_moveStartMouseY;
	std::vector<MovingItem> m_movingNodes;

	std::vector<SelectionEntry> m_selectedNodes;
	PendingLinkItem m_pendingLink;
};


#endif // H_NODEAREA
