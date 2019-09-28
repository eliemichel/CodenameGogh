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


using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
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

private:
	std::shared_ptr<FlowScene> m_scene;
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
  m_scene = std::make_shared<FlowScene>(registerDataModels(), this);
  l->addWidget(new FlowView(m_scene.get()));
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  QObject::connect(saveAction, &QAction::triggered,
                   m_scene.get(), &FlowScene::save);

  QObject::connect(loadAction, &QAction::triggered,
                   m_scene.get(), &FlowScene::load);

  setCentralWidget(centralWidget);
  setWindowTitle("Gogh");
  resize(800, 600);
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
