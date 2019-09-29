#pragma once

#include <memory>

#include <QMainWindow>

#include <nodes/Node>

#include "RenderCommand.h"

using QtNodes::Node;

class GoghFlowScene;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);

private slots:
	void onNodeCreated(Node &n);
	void startRenderInDialog(const RenderCommand & cmd);

private:
	std::shared_ptr<GoghFlowScene> m_scene;
};

