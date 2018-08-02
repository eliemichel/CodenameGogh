#ifndef H_NODEGRAPHICSITEM
#define H_NODEGRAPHICSITEM

#include <QPointF>
#include <QGraphicsRectItem>
#include <QModelIndex>
#include <QObject>

#include <vector>

class NodeWidget;
class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsProxyWidget;
class SlotGraphicsItem;

class NodeGraphicsItem : public QObject
{
	Q_OBJECT
public:
	NodeGraphicsItem(QGraphicsScene *scene, NodeWidget *content);

	const std::vector<SlotGraphicsItem*> & slotItems() const { return m_slotItems; }

	const QModelIndex & modelIndex() const { return m_modelIndex; }
	void setModelIndex(const QModelIndex & modelIndex);

	void setPos(QPointF pos) { if (m_control) m_control->setPos(pos); }

	void setSelected(bool selected);
	bool isSelected() const { return m_isSelected; }

	void updateLinks() const;

protected:
	//QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private slots:
	void onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);

private:
	QGraphicsRectItem *m_control;
	NodeWidget * m_content;
	QGraphicsProxyWidget *m_proxy;
	std::vector<SlotGraphicsItem*> m_slotItems;
	QModelIndex m_modelIndex;
	bool m_isSelected;
};

#endif // H_NODEGRAPHICSITEM
