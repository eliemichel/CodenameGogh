#ifndef H_NODEGRAPHVIEW
#define H_NODEGRAPHVIEW

#include <QGraphicsView>
#include <QModelIndex>
#include <QItemSelection>
#include <vector>

class Slot;
class LinkGraphicsItem;
class SlotGraphicsItem;
class NodeGraphicsItem;
class QAbstractItemModel;
class QItemSelectionModel;
class NodeGraphScene;

/**
 * Node view, handles zooming/panning
 */
class NodeGraphView : public QGraphicsView
{
	Q_OBJECT

private:
	enum Tool {
		DefaultTool,
		CutTool,
	};

private:
	struct NodeMoveData {
		QModelIndex posXIndex;
		QModelIndex posYIndex;
		QPointF startPos;

		NodeMoveData(const QModelIndex & _posXIndex, const QModelIndex & _posYIndex, QPointF _startPos)
			: posXIndex(_posXIndex), posYIndex(_posYIndex), startPos(_startPos)
		{}
	};

public:
	explicit NodeGraphView(QWidget *parent = 0);
	~NodeGraphView();

	QAbstractItemModel * model() const { return m_model; }
	void setModel(QAbstractItemModel *model);

	QItemSelectionModel * selectionModel() const { return m_selectionModel; }
	void setSelectionModel(QItemSelectionModel *selectionModel);

	NodeGraphScene * nodeGraphScene() const;
	void setScene(NodeGraphScene *scene);

protected:
	void drawBackground(QPainter *painter, const QRectF &rect) override;

	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

private:
	void setScene(QGraphicsScene *scene) {} // remove from public API

	void startCut(QPoint position);
	void updateCut(QPoint position);
	void finishCut();

	void selectAll();

private slots:
	void onDataChanged();
	void onCurrentChanged();
	void onSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

private:
	QAbstractItemModel *m_model;
	QItemSelectionModel *m_selectionModel;
	std::vector<NodeMoveData> m_nodeMoveData;

	float m_zoom;
	bool m_isPanning;
	bool m_isCutting;

	// no need for initialization
	QPoint m_pressPos;
	QPointF m_pressCenter;

	/// Links that are currently being dragged
	std::vector<LinkGraphicsItem*> m_pendingLinks;
	std::vector<SlotGraphicsItem*> m_pendingLinksSources;

	// Data for moving nodes
	bool m_isMovingNodes;
	QPoint m_moveStartPos;

	Tool m_currentTool;
	QPainterPath m_cutShape;
};

#endif // H_NODEGRAPHVIEW
