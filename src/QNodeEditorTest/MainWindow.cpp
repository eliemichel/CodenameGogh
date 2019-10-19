
#include <QApplication>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QFileDialog>
#include <QDesktopServices>

#include <nodes/Node>
#include <nodes/DataModelRegistry>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "dialogs/AboutDialog.h"
#include "dialogs/RenderDialog.h"
#include "dialogs/WelcomeDialog.h"
#include "widgets/GoghFlowView.h"
#include "RenderCommand.h"
#include "GoghFlowScene.h"

#include "nodes/FileInputDataModel.h"
#include "nodes/FileOutputDataModel.h"
#include "nodes/H264CodecDataModel.h"
#include "nodes/AacCodecDataModel.h"

using QtNodes::DataModelRegistry;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<FileInputDataModel>("I/O");
  ret->registerModel<FileOutputDataModel>("I/O");
  ret->registerModel<H264CodecDataModel>("Video Codec");
  ret->registerModel<AacCodecDataModel>("Audio Codec");

  return ret;
}


static
QByteArray
defaultScene()
{
  return R"(
  {"nodes": [
    {
      "id": "{12345678-abcd-ef01-2345-0123456789ab}",
      "model": { "name": "File Output", "file": "" },
      "position": { "x": 600, "y": 330 }
    },
    {
      "id": "{01234567-abcd-ef01-2345-0123456789ab}",
      "model": { "name": "File Input", "file": "" },
      "position": { "x": 90, "y": 110 }
    }
  ]}
  )";
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow())
	, m_scene(std::make_unique<GoghFlowScene>(registerDataModels(), this))
{
	// 1. Setup UI

	ui->setupUi(this);
  
	ui->statusBar->addWidget(new QLabel("Add node: Mouse Right or Drop file  |  Box select: Shift + Mouse Left"));

	auto flowView = new GoghFlowView(m_scene.get());

	setCentralWidget(flowView);
	setWindowTitle("Gogh -- DEVELOPMENT VERSION");
	resize(1000, 600);

	// 2. Signals

	/////////// File Menu
  
	connect(
		ui->actionSave, &QAction::triggered,
		this, &MainWindow::save);

	connect(
		ui->actionSaveAs, &QAction::triggered,
		this, &MainWindow::saveAs);

	connect(
		ui->actionOpen, &QAction::triggered,
		m_scene.get(), &FlowScene::load);

	// TODO: why does it crash on quit? It does not when using the window cross
	connect(
		ui->actionQuit, &QAction::triggered,
		this, &QMainWindow::close);

	/////////// Edit Menu

	connect(
		ui->actionCopy, &QAction::triggered,
		flowView, &GoghFlowView::copy);

	connect(
		ui->actionPaste, &QAction::triggered,
		flowView, &GoghFlowView::paste);

	connect(
		ui->actionSelectAll, &QAction::triggered,
		m_scene.get(), &GoghFlowScene::selectAll);

	/////////// Help Menu

	connect(
		ui->actionDocumentation, &QAction::triggered,
		this, &MainWindow::openDocumentation);

	connect(
		ui->actionAbout, &QAction::triggered,
		this, &MainWindow::openAboutDialog);

	/////////// Scene signals

	connect(
		m_scene.get(), &FlowScene::nodeCreated,
		this, &MainWindow::onNodeCreated);

	connect(
		m_scene.get(), &GoghFlowScene::fileStatusChanged,
		this, &MainWindow::onSceneFileStatusChanged);

	// 3. Startup

	WelcomeDialog(this).exec();

	m_scene->loadFromMemory(defaultScene());
	onSceneFileStatusChanged("", true);
}

bool MainWindow::writeScene(const QString & filename)
{
	if (!m_scene || filename.isEmpty()) return false;
	auto scene = m_scene.get();

	QString suffix = "";
	if (!filename.endsWith(".gog", Qt::CaseInsensitive)) {
		suffix = ".gog";
	}

	QFile file(filename + suffix);
	if (!file.open(QIODevice::WriteOnly)) return false;

	file.write(scene->FlowScene::saveToMemory());
	scene->wasSavedAs(filename);
	return true;
}

void MainWindow::save()
{
	if (!m_scene) return;
	auto scene = m_scene.get();
	
	QString filename = scene->filename();
	if (filename.isEmpty()) {
		saveAs();
	} else {
		writeScene(filename);
	}
}

void MainWindow::saveAs()
{
	if (!m_scene) return;
	auto scene = m_scene.get();
	
	QString filename = QFileDialog::getSaveFileName(
		nullptr,
		tr("Save Gogh Graph"),
		QDir::homePath(),
		tr("Gogh Files (*.gog)"));

	writeScene(filename);
}

void MainWindow::openDocumentation()
{
	QDesktopServices::openUrl(QUrl("https://github.com/eliemichel/CodenameGogh")); // TODO: unhardcode-me
}

void MainWindow::openAboutDialog()
{
	AboutDialog(this).exec();
}

void MainWindow::onNodeCreated(Node &n)
{
	QString name = n.nodeDataModel()->name();
	if (name == FileOutputDataModel::staticName()) {
		auto model = static_cast<FileOutputDataModel*>(n.nodeDataModel());
		connect(model, &FileOutputDataModel::renderReady, this, &MainWindow::startRenderInDialog);
	}
}

void MainWindow::onSceneFileStatusChanged(const QString & filename, bool hasBeenModified)
{
	QString shortName = filename.isEmpty() ? "untitled" : QFileInfo(filename).fileName();
	setWindowTitle("Gogh -- DEVELOPMENT VERSION - " + shortName + (hasBeenModified ? "*" : ""));
}

void MainWindow::startRenderInDialog(const RenderCommand & cmd)
{
	RenderDialog(cmd, this).exec();
}
