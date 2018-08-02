#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Logger.h"
#include "NodeGraphModel.h"
#include "NodeWidget.h"
#include "LinkGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "SlotGraphicsItem.h"

#include <QFileDialog>
#include <QGraphicsScene>

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGraphicsProxyWidget>
#include <QHBoxLayout>
#include <QDir>
#include <QModelIndex>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->quitAction, &QAction::triggered, this, &QWidget::close);
	connect(ui->openAction, &QAction::triggered, this, &MainWindow::showOpenFileDialog);
	connect(ui->saveAction, &QAction::triggered, this, &MainWindow::save);

	m_scene = new QGraphicsScene();
	m_model = new NodeGraphModel();

	QString filename = QDir::tempPath() + "/gogh_sample.gog";
	if (!m_model->LoadGraph(filename))
	{
		m_model->LoadDefaultGraph();
		m_model->SaveGraph(filename);
	}

	// Node creation has been moved to NodeGraphModel::LoadGraph
	NodeGraphicsItem *nodeItem;
	int i = 0;
	for (NodeGraphModel::NodeEntry entry : m_model->nodes())
	{
		nodeItem = new NodeGraphicsItem(m_scene, entry.node);
		nodeItem->setModelIndex(m_model->index(i++, 0));
		nodeItem->setPos(QPointF(entry.x, entry.y));
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
}

void MainWindow::showOpenFileDialog()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Gogh Graph"), "", tr("Gogh files (*.gog)"));
	m_model->LoadGraph(filename);
}

void MainWindow::save()
{
	QString filename = QDir::tempPath() + "/gogh_sample.gog";
	m_model->SaveGraph(filename);
}
