#ifndef H_ABSTRACTGRAPHTOOL
#define H_ABSTRACTGRAPHTOOL

#include <QPoint>

class NodeGraphView;

class AbstractGraphTool
{
public:
	AbstractGraphTool(NodeGraphView *view) : m_view(view) {}

	virtual void start(QPoint position) = 0;
	virtual void update(QPoint position) = 0;
	virtual void finish(QPoint position) = 0;

	bool isActive() const { return m_isActive; }

protected:
	void setActive(bool isActive) { m_isActive = isActive; }
	NodeGraphView *view() { return m_view; }

private:
	bool m_isActive = false;
	NodeGraphView *m_view;
};

#endif // H_ABSTRACTGRAPHTOOL
