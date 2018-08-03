#ifndef H_LINKGRAPHICSITEM
#define H_LINKGRAPHICSITEM

#include <QGraphicsItem>

class SlotGraphicsItem;

class LinkGraphicsItem : public QGraphicsItem
{
public:
	explicit LinkGraphicsItem(QGraphicsItem *parent = nullptr);

	void setStartPos(QPointF pos);
	void setEndPos(QPointF pos);

	SlotGraphicsItem * endSlotItem() const { return m_endSlotItem; }
	void setEndSlotItem(SlotGraphicsItem * slotItem) { m_endSlotItem = slotItem; }

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	QPainterPath shape() const override;

private:
	void updateShape();

private:
	QPointF m_startPos;
	QPointF m_endPos;
	QPainterPath m_shape;
	SlotGraphicsItem *m_endSlotItem;
};

#endif // H_LINKGRAPHICSITEM
