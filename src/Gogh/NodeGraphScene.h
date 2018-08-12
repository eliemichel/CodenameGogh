#ifndef H_NODEGRAPHSCENE
#define H_NODEGRAPHSCENE

#include <QGraphicsScene>
#include <QModelIndex>

#include <vector>

class NodeGraphicsItem;
class SlotGraphicsItem;
class LinkGraphicsItem;
class NodeGraphModel;
class Node;

class NodeGraphScene : public QGraphicsScene
{
	Q_OBJECT

public:
	enum CustomData {
		RoleData,
		NodePointerData,
	};
	enum ItemRole {
		NoneRole,
		NodeControlRole,
		NodeContentRole,
		LinkRole,
		SlotRole,
	};
	enum Layer {
		LinkLayer,
		NodeLayer,
		SlotLayer,
	};

public:
	NodeGraphicsItem * toNodeItem(QGraphicsItem *item) const;
	SlotGraphicsItem * toSlotItem(QGraphicsItem *item) const;
	LinkGraphicsItem * toLinkItem(QGraphicsItem *item) const;

	NodeGraphicsItem * nodeItemAtIndex(int index);

	void setGraphModel(NodeGraphModel *model);

	void addNodeItem(Node *node);

private slots:
	void onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
	void onRowsRemoved(const QModelIndex & parent, int first, int last);

private:
	NodeGraphModel * m_nodeGraphModel;
	std::vector<NodeGraphicsItem*> m_nodeItems;
};

#endif // H_NODEGRAPHSCENE
