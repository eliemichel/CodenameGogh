#pragma once

#include <nodes/FlowView>

using QtNodes::FlowView;
using QtNodes::FlowScene;
class GoghFlowScene;

class GoghFlowView : public FlowView {
	Q_OBJECT
public:
	GoghFlowView(QWidget *parent = nullptr);
	GoghFlowView(GoghFlowScene *scene, QWidget *parent = nullptr);

	void setScene(GoghFlowScene *scene);

protected:
	void keyPressEvent(QKeyEvent *event) override;

	void keyReleaseEvent(QKeyEvent *event) override;

	void dragEnterEvent(QDragEnterEvent *event) override;

	void dragMoveEvent(QDragMoveEvent *event) override;

	void dropEvent(QDropEvent *event) override;

protected:
	GoghFlowScene * goghScene();

private:
	void copy();
	void paste();

	void setScene(FlowScene *) {} // shadow original, replaced by the GoghFlowScene version
};
