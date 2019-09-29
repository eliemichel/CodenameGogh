#include <iostream>

#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include <nodes/FlowScene>

#include "GoghFlowView.h"
#include "GoghFlowScene.h"

GoghFlowView::GoghFlowView(QWidget *parent)
	: FlowView(parent)
{}

GoghFlowView::GoghFlowView(GoghFlowScene *scene, QWidget *parent)
	: FlowView(scene, parent)
{}

void GoghFlowView::setScene(GoghFlowScene *scene) {
	FlowView::setScene(scene);
}

void GoghFlowView::keyPressEvent(QKeyEvent *event) {
	std::cout << "keyPressEvent" << std::endl;
	switch (event->key()) {
	case Qt::Key_C:
		std::cout << "C" << std::endl;
		if (event->modifiers() & Qt::ControlModifier) {
			std::cout << "Ctrl+C" << std::endl;
			copy();
			return;
		}
		break;

	case Qt::Key_V:
		if (event->modifiers() & Qt::ControlModifier) {
			paste();
			return;
		}
		break;

	default:
		break;
	}

	FlowView::keyPressEvent(event);
}

void GoghFlowView::keyReleaseEvent(QKeyEvent *event) {
	FlowView::keyReleaseEvent(event);
}

GoghFlowScene * GoghFlowView::goghScene() {
	return static_cast<GoghFlowScene*>(FlowView::scene());
}

void GoghFlowView::copy() {
	GoghFlowScene *gscene = goghScene();
	QClipboard *clipboard = QApplication::clipboard();
	QMimeData *mimeData = new QMimeData();
	QByteArray data = gscene->saveToMemory(gscene->selectedNodes());
	mimeData->setData("x-gogh/nodes", data);
	mimeData->setText(data);
	clipboard->setMimeData(mimeData);
}

void GoghFlowView::paste() {
	GoghFlowScene *gscene = goghScene();

	const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
	if (mimeData->hasFormat("x-gogh/nodes")) {
		gscene->loadFromMemory(mimeData->data("x-gogh/nodes"), true /* newIds */);
	} else if (mimeData->hasText()) {
		gscene->loadFromMemory(mimeData->text().toUtf8(), true /* newIds */);
	}

	// TODO: set selection to newly created nodes
}
