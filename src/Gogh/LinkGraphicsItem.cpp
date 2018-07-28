#include "LinkGraphicsItem.h"

void LinkGraphicsItem::setStartPos(QPointF pos) {
	QLineF l = line();
	l.setP1(pos);
	setLine(l);
}

void LinkGraphicsItem::setEndPos(QPointF pos) {
	QLineF l = line();
	l.setP2(pos);
	setLine(l);
}

