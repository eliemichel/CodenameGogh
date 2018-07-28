#ifndef H_NODEGRAPHVIEW
#define H_NODEGRAPHVIEW

#include <QGraphicsView>
#include <vector>

class Slot;
class LinkGraphicsItem;
class SlotGraphicsItem;

/**
 * Node view, handles zooming/panning
 */
class NodeGraphView : public QGraphicsView
{
	Q_OBJECT

public:
	enum CustomData {
		RoleData,
	};
	enum ItemRole {
		NoneRole,
		NodeControlRole,
		NodeContentRole,
		SlotRole,
	};

public:
	explicit NodeGraphView(QWidget *parent = 0);
	~NodeGraphView();

protected:
	void drawBackground(QPainter *painter, const QRectF &rect) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

private:
	float m_zoom;
	bool m_isPanning;

	// no need for initialization
	QPoint m_pressPos;
	QPointF m_pressCenter;

	/// Links that are currently being dragged
	std::vector<LinkGraphicsItem*> m_pendingLinks;
	std::vector<SlotGraphicsItem*> m_pendingLinksSources;
};

#endif // H_NODEGRAPHVIEW
