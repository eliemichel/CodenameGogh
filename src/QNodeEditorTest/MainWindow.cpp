
#include <QApplication>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>

#include <nodes/Node>
#include <nodes/DataModelRegistry>

#include "MainWindow.h"
#include "dialogs/RenderDialog.h"
#include "widgets/GoghFlowView.h"
#include "RenderCommand.h"
#include "GoghFlowScene.h"

#include "nodes/FileInputDataModel.h"
#include "nodes/FileOutputDataModel.h"

using QtNodes::DataModelRegistry;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<FileInputDataModel>("I/O");
  ret->registerModel<FileOutputDataModel>("I/O");

  return ret;
}


static
QByteArray
defaultScene()
{
  return R"(
  {"nodes": [{
    "id": "{12345678-abcd-ef01-2345-0123456789ab}",
    "model": { "name": "File Output", "file": "" },
    "position": { "x": 411, "y": 93 }
  }]}
  )";
}


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
  auto menuBar = new QMenuBar();
  auto saveAction = menuBar->addAction("Save..");
  auto loadAction = menuBar->addAction("Load..");

  auto statusBar = new QStatusBar();
  statusBar->addWidget(new QLabel("Add node: Mouse Right or Drop file  |  Box select: Shift + Mouse Left"));

  auto centralWidget = new QWidget();
  QVBoxLayout *l = new QVBoxLayout(centralWidget);

  l->addWidget(menuBar);
  m_scene = std::make_shared<GoghFlowScene>(registerDataModels(), this);
  m_scene->loadFromMemory(defaultScene());
  l->addWidget(new GoghFlowView(m_scene.get()));
  l->addWidget(statusBar);
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  QObject::connect(saveAction, &QAction::triggered,
                   m_scene.get(), &FlowScene::save);

  QObject::connect(loadAction, &QAction::triggered,
                   m_scene.get(), &FlowScene::load);

  QObject::connect(m_scene.get(), &FlowScene::nodeCreated,
                   this, &MainWindow::onNodeCreated);

  setCentralWidget(centralWidget);
  setWindowTitle("Gogh -- DEVELOPMENT VERSION");
  resize(1000, 600);
}

void MainWindow::onNodeCreated(Node &n) {
	QString name = n.nodeDataModel()->name();
	if (name == FileOutputDataModel::staticName()) {
		auto model = static_cast<FileOutputDataModel*>(n.nodeDataModel());
		connect(model, &FileOutputDataModel::renderReady, this, &MainWindow::startRenderInDialog);
	}
}

void MainWindow::startRenderInDialog(const RenderCommand & cmd) {
	RenderDialog dialog(cmd, this);
	dialog.exec();
}
