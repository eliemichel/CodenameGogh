#ifndef H_NODEGRAPHVIEW
#define H_NODEGRAPHVIEW

#include <QGraphicsView>
#include <vector>

class Slot;
class LinkGraphicsItem;
class SlotGraphicsItem;
class NodeGraphicsItem;
class QAbstractItemModel;
class QItemSelectionModel;

/**
 * Node view, handles zooming/panning
 */
class NodeGraphView : public QGraphicsView
{
	Q_OBJECT

public:
	enum CustomData {
		RoleData,
		NodePointerData,
	};
	enum ItemRole {
		NoneRole,
		NodeControlRole,
		NodeContentRole,
		SlotRole,
	};

private:
	struct SelectionItem {
		NodeGraphicsItem *nodeItem;
		QPointF startPos;

		SelectionItem(NodeGraphicsItem *_nodeItem, QPointF _startPos) : nodeItem(_nodeItem), startPos(_startPos) {}
	};

public:
	explicit NodeGraphView(QWidget *parent = 0);
	~NodeGraphView();

	QAbstractItemModel * model() const { return m_model; }
	void setModel(QAbstractItemModel *model);

	QItemSelectionModel * selectionModel() const { return m_selectionModel; }
	void setSelectionModel(QItemSelectionModel *selectionModel);

protected:
	void drawBackground(QPainter *painter, const QRectF &rect) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

private slots:
	void onDataChanged();
	void onCurrentChanged();

private:
	QAbstractItemModel *m_model;
	QItemSelectionModel *m_selectionModel;
	std::vector<QPointF> m_selectionStartPos;

	float m_zoom;
	bool m_isPanning;

	// no need for initialization
	QPoint m_pressPos;
	QPointF m_pressCenter;

	/// Links that are currently being dragged
	std::vector<LinkGraphicsItem*> m_pendingLinks;
	std::vector<SlotGraphicsItem*> m_pendingLinksSources;

	// Data for moving nodes
	bool m_isMovingNodes;
	QPoint m_moveStartPos;
};

#endif // H_NODEGRAPHVIEW
