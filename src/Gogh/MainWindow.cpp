#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Logger.h"
#include "NodeWidget.h"
#include "LinkGraphicsItem.h"

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

	NodeWidget *leftNode = new NodeWidget(G_TEST_LEFTNODE);
	NodeWidget *rightNode = new NodeWidget(G_TEST_RIGHTNODE);

	QGraphicsProxyWidget *wdg;
	wdg = m_scene->addWidget(leftNode);
	wdg->setData(0, 1); // set item as node proxy TODO: use enums
	wdg->setPos(-200, 0);

	wdg = m_scene->addWidget(rightNode);
	wdg->setData(0, 1);
	wdg->setPos(200, 0);

	LinkGraphicsItem *link = new LinkGraphicsItem();
	link->setLine(0, 0, 120, 150);
	m_scene->addItem(link);
	leftNode->allSlots()[0]->addOutputLink(link);
	rightNode->allSlots()[0]->addInputLink(link);

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
