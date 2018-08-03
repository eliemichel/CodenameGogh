#ifndef H_PANTOOL
#define H_PANTOOL

#include "AbstractGraphTool.h"

#include <QPointF>

class PanTool : public AbstractGraphTool
{
public:
	PanTool(NodeGraphView *view) : AbstractGraphTool(view) {}

	void start(QPoint position) override;
	void update(QPoint position) override;
	void finish(QPoint position) override;

private:
	QPoint m_pressPos;
	QPointF m_pressCenter;
};

#endif // H_PANTOOL
