#ifndef H_NODEGRAPHSCENE
#define H_NODEGRAPHSCENE

#include <QGraphicsScene>

class NodeGraphicsItem;
class SlotGraphicsItem;

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

	NodeGraphicsItem * nodeItemAtIndex(const QModelIndex & index);
};

#endif // H_NODEGRAPHSCENE
