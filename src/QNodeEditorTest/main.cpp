#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/ConnectionStyle>
#include <nodes/TypeConverter>

#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMenuBar>

#include <nodes/DataModelRegistry>

#include "nodes/NumberSourceDataModel.h"
#include "nodes/NumberDisplayDataModel.h"
#include "nodes/FileInputDataModel.h"
#include "nodes/FileOutputDataModel.h"
#include "dialogs/RenderDialog.h"
#include "widgets/GoghFlowView.h"
#include "RenderCommand.h"
#include "GoghFlowScene.h"


using QtNodes::DataModelRegistry;
using QtNodes::ConnectionStyle;
using QtNodes::TypeConverter;
using QtNodes::TypeConverterId;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<NumberSourceDataModel>("Sources");

  ret->registerModel<NumberDisplayDataModel>("Displays");

  ret->registerModel<FileInputDataModel>("Gogh");
  ret->registerModel<FileOutputDataModel>("Gogh");

  return ret;
}


static
void
setStyle()
{
  ConnectionStyle::setConnectionStyle(
  R"(
  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "black",
      "SelectedColor": "gray",
      "SelectedHaloColor": "deepskyblue",
      "HoveredColor": "deepskyblue",

      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,

      "UseDataDefinedColors": true
    }
  }
  )");
}


#include <QMainWindow>

using QtNodes::Node;

class MainWindow : public QMainWindow {
public:
	MainWindow(QWidget *parent = nullptr);

private slots:
	void onNodeCreated(Node &n);
	void startRenderInDialog(const RenderCommand & cmd);

private:
	std::shared_ptr<GoghFlowScene> m_scene;
};

#include <nodes/Node>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
  auto menuBar    = new QMenuBar();
  auto saveAction = menuBar->addAction("Save..");
  auto loadAction = menuBar->addAction("Load..");

  auto centralWidget = new QWidget();
  QVBoxLayout *l = new QVBoxLayout(centralWidget);

  l->addWidget(menuBar);
  m_scene = std::make_shared<GoghFlowScene>(registerDataModels(), this);
  l->addWidget(new GoghFlowView(m_scene.get()));
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  QObject::connect(saveAction, &QAction::triggered,
                   m_scene.get(), &FlowScene::save);

  QObject::connect(loadAction, &QAction::triggered,
                   m_scene.get(), &FlowScene::load);

  QObject::connect(m_scene.get(), &FlowScene::nodeCreated,
                   this, &MainWindow::onNodeCreated);

  setCentralWidget(centralWidget);
  setWindowTitle("Gogh");
  resize(800, 600);
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

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  setStyle();

  MainWindow w;
  w.showNormal();
  
  return app.exec();
}
