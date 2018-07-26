#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Logger.h"

#include <QFileDialog>
#include <QGraphicsScene>

#include <QPushButton>
#include <QGraphicsProxyWidget>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->actionQuit, &QAction::triggered, this, &QWidget::close);
	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::showOpenFileDialog);

	m_scene = new QGraphicsScene();
	m_scene->addText("Hello, world!");
	QGraphicsProxyWidget *wdg = m_scene->addWidget(new QPushButton("TEST"));
	wdg->setPos(500, 0);
	wdg = m_scene->addWidget(new QPushButton("TEST"));
	wdg->setPos(-500, 0);
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
