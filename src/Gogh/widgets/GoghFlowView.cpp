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
#include <QRubberBand>

#include <nodes/FlowScene>

#include "GoghFlowView.h"
#include "GoghFlowScene.h"


GoghFlowView::GoghFlowView(GoghFlowScene *scene, QWidget *parent)
	: FlowView(scene, parent)
{
	setTransformationAnchor(ViewportAnchor::AnchorUnderMouse);
	setDragMode(NoDrag); // we implement our own
	setFocusPolicy(Qt::ClickFocus);
	setAcceptDrops(true);
}

void GoghFlowView::setScene(GoghFlowScene *scene) {
	FlowView::setScene(scene);
}


// ----------------------------------------------------------------------------

void GoghFlowView::mousePressEvent(QMouseEvent* event)
{
	bool navig = (event->button() == Qt::MiddleButton) || (((event->button() == Qt::LeftButton) && (event->modifiers() & Qt::AltModifier) != 0));
	if (navig)
	{
		m_clickPos = mapToScene(event->pos());
		setCursor(QCursor(Qt::ClosedHandCursor));
		return;
	}

	QGraphicsView::mousePressEvent(event);

	bool rubberband = event->button() == Qt::LeftButton && !navig && !event->isAccepted();
	if (rubberband)
	{
		m_clickPos = mapToScene(event->pos());
		if (!m_rubberBand) {
			m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
		}
		m_rubberBand->setGeometry(QRect(event->pos(), QSize()));
		m_rubberBand->show();
	}
}

void GoghFlowView::mouseMoveEvent(QMouseEvent* event)
{
	FlowView::mouseMoveEvent(event);
	bool navig = (event->buttons() == Qt::MiddleButton) || (((event->buttons() == Qt::LeftButton) && (event->modifiers() & Qt::AltModifier) != 0));
	if (scene() && scene()->mouseGrabberItem() == nullptr && navig)
	{
		// Make sure shift is not being pressed
		if ((event->modifiers() & Qt::ShiftModifier) == 0)
		{
			QPointF difference = m_clickPos - mapToScene(event->pos());
			setSceneRect(sceneRect().translated(difference.x(), difference.y()));
		}
	}

	if (m_rubberBand != nullptr && m_rubberBand->isVisible())
	{
		m_rubberBand->setGeometry(QRect(mapFromScene(m_clickPos), event->pos()).normalized());
	}
}

void GoghFlowView::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_rubberBand != nullptr && m_rubberBand->isVisible())
	{
		if (scene())
		{
			QPointF releasePos = mapToScene(event->pos());
			QPainterPath path;
			path.addRect(QRectF(m_clickPos, releasePos));
			scene()->setSelectionArea(path, Qt::IntersectsItemBoundingRect);
			for (const auto& i : scene()->selectedItems())
			{
				// TODO: fix selection hitbox
			}
		}
		m_rubberBand->hide();
	}
	if ((event->modifiers() & Qt::AltModifier) != 0)
	{
		setCursor(QCursor(Qt::OpenHandCursor));
	}
	else
	{
		setCursor(QCursor(Qt::ArrowCursor));
	}
	QGraphicsView::mouseReleaseEvent(event);
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
