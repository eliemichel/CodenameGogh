#ifndef H_LINKGRAPHICSITEM
#define H_LINKGRAPHICSITEM

#include <QGraphicsItem>
#include <QGraphicsLineItem>

class LinkGraphicsItem : public QGraphicsLineItem
{
public:
	void setStartPos(QPointF pos);
	void setEndPos(QPointF pos);
};

#endif // H_LINKGRAPHICSITEM
