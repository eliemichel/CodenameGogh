#pragma once

#include <memory>

#include <QMainWindow>

#include <nodes/Node>

#include "RenderCommand.h"
#include "GoghFlowScene.h"

using QtNodes::Node;

class GoghFlowScene;

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);

public slots:
	void openFile();

private:
	/**
	 * Utility function used by both save() and saveAs() to actually write the
	 * scene file. This fonctions signals the scene that it has been saved and
	 * updates its filename.
	 * @return status
	 */
	bool writeScene(const QString & filename);

private slots:
	void save();
	void saveAs();
	void openDocumentation();
	void openAboutDialog();

	void onNodeCreated(Node &n);
	void onSceneFileStatusChanged(const QString & filename, bool hasBeenModified);
	void startRenderInDialog(const RenderCommand & cmd);

private:
	Ui::MainWindow *ui;

	/**
	 * The scene pointer should never be null. If no scene is openned, it
	 * points to an empty scene, not to null.
	 */
	std::unique_ptr<GoghFlowScene> m_scene;
};

