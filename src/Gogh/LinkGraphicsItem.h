#ifndef H_LINKGRAPHICSITEM
#define H_LINKGRAPHICSITEM

#include <QGraphicsItem>

class LinkGraphicsItem : public QGraphicsItem
{
public:
	void setStartPos(QPointF pos);
	void setEndPos(QPointF pos);

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const override;
	QPainterPath shape() const override;

private:
	QPointF m_startPos;
	QPointF m_endPos;
};

#endif // H_LINKGRAPHICSITEM
