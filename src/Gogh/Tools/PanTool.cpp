#include "PanTool.h"

#include "NodeGraphView.h"

void PanTool::start(QPoint position)
{
	setActive(true);
	m_pressPos = position;
	m_pressCenter = view()->mapToScene(view()->viewport()->rect().center());
}

void PanTool::update(QPoint position)
{
	QPointF c = view()->mapToScene(view()->viewport()->rect().center());
	QPointF delta = view()->mapToScene(m_pressPos) - view()->mapToScene(position);
	view()->centerOn(m_pressCenter + delta);
}

void PanTool::finish(QPoint position)
{
	setActive(false);
}
