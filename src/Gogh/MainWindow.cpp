#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Logger.h"
#include "NodeWidget.h"
#include "LinkGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "SlotGraphicsItem.h"
#include "Nodes/InputNode.h"
#include "Nodes/OutputNode.h"
#include "Nodes/ScaleNode.h"

#include <QFileDialog>
#include <QGraphicsScene>

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGraphicsProxyWidget>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->actionQuit, &QAction::triggered, this, &QWidget::close);
	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::showOpenFileDialog);

	m_scene = new QGraphicsScene();

	NodeGraphicsItem *nodeItem;
	nodeItem = new NodeGraphicsItem(m_scene, new InputNode());
	nodeItem->setPos(QPointF(-300, -200));
	nodeItem = new NodeGraphicsItem(m_scene, new ScaleNode());
	nodeItem->setPos(QPointF(0, -250));
	nodeItem = new NodeGraphicsItem(m_scene, new OutputNode());
	nodeItem->setPos(QPointF(300, -200));

	m_scene->setSceneRect(-1000, -1000, 2000, 2000);
	ui->viewport->setScene(m_scene);

	ui->splitter->setSizes(QList<int>() << 200 << 100);
}

MainWindow::~MainWindow()
{
}

void MainWindow::showOpenFileDialog()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Shader"), "", tr("Shader Files (*.txt *.glsl)"));
}
