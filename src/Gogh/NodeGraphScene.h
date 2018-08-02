#ifndef H_NODEGRAPHSCENE
#define H_NODEGRAPHSCENE

#include <QGraphicsScene>

class NodeGraphicsItem;

class NodeGraphScene : public QGraphicsScene
{
	Q_OBJECT

public:
	NodeGraphicsItem * toNodeItem(QGraphicsItem *item) const;

	NodeGraphicsItem * nodeItemAtIndex(const QModelIndex & index);
};

#endif // H_NODEGRAPHSCENE
