#pragma once

#include <nodes/FlowView>

using QtNodes::FlowView;
using QtNodes::FlowScene;
class GoghFlowScene;
class QRubberBand;

class GoghFlowView : public FlowView {
	Q_OBJECT
public:
	GoghFlowView(GoghFlowScene *scene, QWidget *parent = nullptr);

	void setScene(GoghFlowScene *scene);

public slots:
	void copy();
	void paste();

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	void keyPressEvent(QKeyEvent *event) override;

	void keyReleaseEvent(QKeyEvent *event) override;

	void dragEnterEvent(QDragEnterEvent *event) override;

	void dragMoveEvent(QDragMoveEvent *event) override;

	void dropEvent(QDropEvent *event) override;

protected:
	GoghFlowScene * goghScene();

private:
	using FlowView::setScene; // make private the raw pointer version

private:
	QPointF m_clickPos;
	QRubberBand* m_rubberBand = nullptr;
};
