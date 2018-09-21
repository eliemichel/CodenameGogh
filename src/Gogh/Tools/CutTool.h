#ifndef H_CUTTOOL
#define H_CUTTOOL

#include "AbstractGraphTool.h"

#include <QPainterPath>

class CutTool : public AbstractGraphTool
{
public:
	CutTool(QGraphicsView *view) : AbstractGraphTool(view) {}

	void start(QPoint position) override;
	void update(QPoint position) override;
	void finish(QPoint position) override;

	const QPainterPath & cutShape() const { return m_cutShape; }

private:
	QPainterPath m_cutShape;
};

#endif // H_CUTTOOL
