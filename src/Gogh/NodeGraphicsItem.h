#ifndef H_NODEGRAPHICSITEM
#define H_NODEGRAPHICSITEM

#include <QPointF>
#include <QGraphicsRectItem>

#include <vector>

class NodeWidget;
class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsProxyWidget;
class SlotGraphicsItem;

class NodeGraphicsItem : public QGraphicsRectItem
{
public:
	NodeGraphicsItem(QGraphicsScene *scene, NodeWidget *content);

	const std::vector<SlotGraphicsItem*> & slotItems() const { return m_slotItems; }

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
	NodeWidget * m_content;
	QGraphicsProxyWidget *m_proxy;
	std::vector<SlotGraphicsItem*> m_slotItems;
};

#endif // H_NODEGRAPHICSITEM
