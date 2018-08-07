#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Logger.h"
#include "NodeGraphModel.h"
#include "NodeGraphScene.h"
#include "NodeWidget.h"
#include "LinkGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "SlotGraphicsItem.h"
#include "EnvModel.h"
#include "Dialogs/EnvDialog.h"

#include <QFileDialog>
#include <QGraphicsScene>

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGraphicsProxyWidget>
#include <QHBoxLayout>
#include <QDir>
#include <QModelIndex>

MainWindow::MainWindow(QString graphFilename, QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m_currentFilename(graphFilename)
{
	//TODO: Temporary fixes the MenuBar issue by setting it in the Mainwindow, try to set back native menubar
	QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);

	ui->setupUi(this);

	connect(ui->quitAction, &QAction::triggered, this, &QWidget::close);
	connect(ui->openAction, &QAction::triggered, this, &MainWindow::showOpenFileDialog);
	connect(ui->saveAction, &QAction::triggered, this, &MainWindow::save);
	connect(ui->saveAsAction, &QAction::triggered, this, &MainWindow::showSaveAsFileDialog);
	connect(ui->envAction, &QAction::triggered, this, &MainWindow::showEnvDialog);

	m_scene = new NodeGraphScene();
	m_model = new NodeGraphModel();
	m_envModel = new EnvModel();

	m_model->setEnvModel(m_envModel);

	if (m_currentFilename.isNull())
	{
		m_currentFilename = QDir::tempPath() + "/gogh_sample.gog";
		if (!m_model->LoadGraph(m_currentFilename))
		{
			m_model->LoadDefaultGraph();
			m_model->SaveGraph(m_currentFilename);
		}
	}
	else
	{
		if (!m_model->LoadGraph(m_currentFilename))
		{
			ERR_LOG << "Unable to load file: " << m_currentFilename.toStdString();
			m_currentFilename = QString();
		}
	}

	// Node creation has been moved to NodeGraphModel::LoadGraph
	NodeGraphicsItem *nodeItem;
	for (int i = 0 ; i < m_model->rowCount() ; ++i)
	{
		float x = m_model->data(m_model->index(i, NodeGraphModel::PosXColumn)).toFloat();
		float y = m_model->data(m_model->index(i, NodeGraphModel::PosYColumn)).toFloat();
		nodeItem = new NodeGraphicsItem(m_scene, m_model->nodeData(i));
		nodeItem->setModelIndex(m_model->index(i, 0));
		nodeItem->setPos(QPointF(x, y));
	}

	m_scene->setSceneRect(-1000, -1000, 2000, 2000);

	ui->splitter->setSizes(QList<int>() << 300 << 10);

	ui->outliner->setModel(m_model);

	ui->parameters->setModel(m_model);
	ui->parameters->setSelectionModel(ui->outliner->selectionModel());

	ui->viewport->setScene(m_scene);
	ui->viewport->setModel(m_model);
	ui->viewport->setSelectionModel(ui->outliner->selectionModel());
}

MainWindow::~MainWindow()
{
	delete m_model;
	delete m_envModel;
	delete m_scene;
}

void MainWindow::showOpenFileDialog()
{
	m_currentFilename = QFileDialog::getOpenFileName(this, tr("Open Gogh Graph"), "", tr("Gogh files (*.gog)"));
	m_model->LoadGraph(m_currentFilename);
}

void MainWindow::save()
{
	if (m_currentFilename.isNull())
	{
		showSaveAsFileDialog();
	}
	else
	{
		m_model->SaveGraph(m_currentFilename);
	}
}

void MainWindow::showSaveAsFileDialog()
{
	m_currentFilename = QFileDialog::getSaveFileName(this, tr("Save as Gogh Graph"), m_currentFilename, tr("Gogh files (*.gog)"));
	m_model->SaveGraph(m_currentFilename);
}

void MainWindow::showEnvDialog()
{
	EnvDialog envDialog(m_envModel);
	envDialog.exec();
}
