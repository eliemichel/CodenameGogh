#ifndef H_NODEGRAPHVIEW
#define H_NODEGRAPHVIEW

#include "Tools/PanTool.h"
#include "Tools/CutTool.h"

#include <QGraphicsView>
#include <QModelIndex>
#include <QItemSelection>
#include <vector>

class LinkGraphicsItem;
class SlotGraphicsItem;
class NodeGraphicsItem;
class NodeGraphModel;
class QItemSelectionModel;
class NodeGraphScene;
class QAction;

/**
 * Node view, handles zooming/panning
 */
class NodeGraphView : public QGraphicsView
{
	Q_OBJECT

private:
	enum ToolState {
		DefaultToolState,
		CutToolState,
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
	
	NodeGraphModel * model() const { return m_model; }
	void setModel(NodeGraphModel *model);

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

	void contextMenuEvent(QContextMenuEvent *event) override;

private:
	void setScene(QGraphicsScene *scene) {} // remove from public API

	void createActions();

	// TODO: move to a tool
	void startMoveNodes(QPoint position);
	void updateMoveNodes(QPoint position);
	void finishMoveNodes();

	void selectAll();

private slots:
	void onSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

private:
	NodeGraphModel *m_model;
	QItemSelectionModel *m_selectionModel;
	std::vector<NodeMoveData> m_nodeMoveData;

	float m_zoom;

	/// Links that are currently being dragged
	std::vector<LinkGraphicsItem*> m_pendingLinks;
	std::vector<SlotGraphicsItem*> m_pendingLinksSources;

	// Data for moving nodes
	bool m_isMovingNodes;
	QPoint m_moveStartPos;

	ToolState m_currentToolState;

	PanTool m_panTool;
	CutTool m_cutTool;

	std::vector<QAction*> m_addNodeActions;
	QPointF m_newNodePos;
};

#endif // H_NODEGRAPHVIEW
