#ifndef H_NODEGRAPHVIEW
#define H_NODEGRAPHVIEW

#include <QGraphicsView>

/**
 * Node view, handles zooming/panning and moving the nodes
 */
class NodeGraphView : public QGraphicsView
{
	Q_OBJECT

public:
	explicit NodeGraphView(QWidget *parent = 0);
	~NodeGraphView();

protected:
	void drawBackground(QPainter *painter, const QRectF &rect) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

private:
	float m_zoom;
	bool m_isPanning;
	bool m_isMovingNode;

	// no need for initialization
	QPoint m_pressPos;
	QPointF m_pressCenter;
	QGraphicsItem *m_movingItem;
	QPointF m_movingItemStartPos;
};

#endif // H_NODEGRAPHVIEW
