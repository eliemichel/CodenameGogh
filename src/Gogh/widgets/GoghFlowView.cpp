#include <iostream>

#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonArray>

#include <nodes/FlowScene>

#include "GoghFlowView.h"
#include "GoghFlowScene.h"

GoghFlowView::GoghFlowView(QWidget *parent)
	: FlowView(parent)
{
	setAcceptDrops(true);
}

GoghFlowView::GoghFlowView(GoghFlowScene *scene, QWidget *parent)
	: FlowView(scene, parent)
{
	setAcceptDrops(true);
}

void GoghFlowView::setScene(GoghFlowScene *scene) {
	FlowView::setScene(scene);
}

// ----------------------------------------------------------------------------

void GoghFlowView::keyPressEvent(QKeyEvent *event) {
	/*
	switch (event->key()) {
	case Qt::Key_C:
		if (event->modifiers() & Qt::ControlModifier) {
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
	*/
	FlowView::keyPressEvent(event);
}

void GoghFlowView::keyReleaseEvent(QKeyEvent *event) {
	FlowView::keyReleaseEvent(event);
}

void GoghFlowView::dragEnterEvent(QDragEnterEvent *event) {
	// TODO: this could be compiled only once
	// and shared with dropEvent; and TODO full list
	QRegularExpression isMediaFile(R"((\.wav|\.mp3|\.mp4|\.mkv|\.m4v|\.png|\.jpg|\.exr)$)");

	if (event->mimeData()->hasFormat("text/uri-list")) {
		QByteArrayList data = event->mimeData()->data("text/uri-list").split('\n');
		for (QByteArray d : data) {
			if (isMediaFile.match(d.trimmed()).hasMatch()) {
				event->acceptProposedAction();
				event->accept();
			}
			return;
		}
	}
}

void GoghFlowView::dragMoveEvent(QDragMoveEvent *event) {
}

void GoghFlowView::dropEvent(QDropEvent *event)
{
	QRegularExpression isMediaFile(R"((\.wav|\.mp3|\.mp4|\.mkv|\.m4v|\.png|\.jpg|\.exr)$)");

	QJsonObject sceneJson;
	QJsonArray nodesJsonArray;
	QPointF p = mapToScene(event->pos());
	int xPos = p.x() - 10;
	int yPos = p.y() - 10;

	if (event->mimeData()->hasFormat("text/uri-list")) {
		QByteArrayList data = event->mimeData()->data("text/uri-list").split('\n');
		for (QByteArray d : data) {
			if (isMediaFile.match(d.trimmed()).hasMatch()) {
				QUrl url(d.trimmed());
				if (url.isLocalFile()) {
					// Build a json object decribing a "File Input" node with the dropped url
					QJsonObject nodeJson;
					nodeJson["id"] = QUuid::createUuid().toString();

					QJsonObject modelJson;
					modelJson["name"] = "File Input";
					modelJson["file"] = url.toLocalFile();
					nodeJson["model"] = modelJson;

					QJsonObject positionJson;
					positionJson["x"] = xPos;
					positionJson["y"] = yPos;
					xPos += 5;
					yPos += 20;
					nodeJson["position"] = positionJson;

					nodesJsonArray.append(nodeJson);

					event->acceptProposedAction();
				}
			}
		}
	}

	sceneJson["nodes"] = nodesJsonArray;
	QJsonDocument document(sceneJson);
	goghScene()->loadFromMemory(document.toJson(), true /* newIds */);
}

// ----------------------------------------------------------------------------

GoghFlowScene * GoghFlowView::goghScene() {
	return static_cast<GoghFlowScene*>(FlowView::scene());
}

// ----------------------------------------------------------------------------

void GoghFlowView::copy() {
	GoghFlowScene *gscene = goghScene();
	QClipboard *clipboard = QApplication::clipboard();
	QMimeData *mimeData = new QMimeData();
	QByteArray data = gscene->saveToMemory(gscene->selectedNodes());
	mimeData->setData("application/x-gogh-nodes", data);
	mimeData->setText(data);
	clipboard->setMimeData(mimeData);
}

void GoghFlowView::paste() {
	GoghFlowScene *gscene = goghScene();

	const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
	if (mimeData->hasFormat("application/x-gogh-nodes")) {
		gscene->loadFromMemory(mimeData->data("application/x-gogh-nodes"), true /* newIds */);
	} else if (mimeData->hasText()) {
		gscene->loadFromMemory(mimeData->text().toUtf8(), true /* newIds */);
	}

	// TODO: set selection to newly created nodes
}
