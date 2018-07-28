#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Logger.h"
#include "NodeWidget.h"
#include "LinkGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "SlotGraphicsItem.h"
#include "Nodes/InputNode.h"
#include "Nodes/OutputNode.h"

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

	NodeWidget *leftNodeContent = new NodeWidget(G_TEST_LEFTNODE);
	NodeWidget *rightNodeContent = new NodeWidget(G_TEST_RIGHTNODE);

	NodeGraphicsItem *leftNodeItem = new NodeGraphicsItem(m_scene, leftNodeContent);
	leftNodeItem->setPos(QPointF(-200, 0));

	NodeGraphicsItem *rightNodeItem = new NodeGraphicsItem(m_scene, rightNodeContent);
	rightNodeItem->setPos(QPointF(200, 0));

	NodeGraphicsItem *nodeItem;
	nodeItem = new NodeGraphicsItem(m_scene, new InputNode());
	nodeItem->setPos(QPointF(-300, -200));
	nodeItem = new NodeGraphicsItem(m_scene, new OutputNode());
	nodeItem->setPos(QPointF(-100, -200));

	SlotGraphicsItem *slotItem = new SlotGraphicsItem();
	slotItem->setPos(100, -200);
	slotItem->setSlot(new Slot());
	m_scene->addItem(slotItem);

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
