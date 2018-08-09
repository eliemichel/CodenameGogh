#ifndef H_NODEGRAPHICSITEM
#define H_NODEGRAPHICSITEM

#include <QPointF>
#include <QGraphicsRectItem>
#include <QModelIndex>
#include <QObject>

#include <vector>

class NodeWidget;
class QGraphicsRectItem;
class QGraphicsProxyWidget;
class SlotGraphicsItem;
class NodeGraphScene;

// TODO: move this in its own file
class NodeGraphicsItemControl : public QGraphicsRectItem
{
public:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	const QString & name() const { return m_name; }
	void setName(const QString & name) { m_name = name; }

private:
	QString m_name;
};

class NodeGraphicsItem : public QObject
{
	Q_OBJECT
public:
	NodeGraphicsItem(NodeGraphScene *scene, NodeWidget *content);

	const QModelIndex & modelIndex() const { return m_modelIndex; }
	void setModelIndex(const QModelIndex & modelIndex);

	void setPos(QPointF pos) { if (m_control) m_control->setPos(pos); }

	void setSelected(bool selected);
	bool isSelected() const { return m_isSelected; }

	void updateInputLinks() const;
	void updateOutputLinks() const;

	QPointF outputSlotPosition(int i) const;

protected:
	//QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private slots:
	void onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);

private:
	NodeGraphScene *m_graphScene;
	NodeGraphicsItemControl *m_control;
	NodeWidget * m_content;
	QGraphicsProxyWidget *m_proxy;
	std::vector<SlotGraphicsItem*> m_outputSlotItems;
	std::vector<SlotGraphicsItem*> m_inputSlotItems;
	QModelIndex m_modelIndex;
	bool m_isSelected;
};

#endif // H_NODEGRAPHICSITEM
