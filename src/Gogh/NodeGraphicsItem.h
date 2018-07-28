#ifndef H_NODEGRAPHICSITEM
#define H_NODEGRAPHICSITEM

#include <QPointF>
#include <QGraphicsRectItem>

class NodeWidget;
class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsProxyWidget;

class NodeGraphicsItem : public QGraphicsRectItem
{
public:
	NodeGraphicsItem(QGraphicsScene *scene, NodeWidget *content);
	void updateLinks() const;

protected:
	QVariant NodeGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value);

private:
	NodeWidget * m_content;
	QGraphicsProxyWidget *m_proxy;
};

#endif // H_NODEGRAPHICSITEM
