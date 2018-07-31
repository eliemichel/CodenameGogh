#ifndef H_MAINWINDOW
#define H_MAINWINDOW

#include <QMainWindow>

class QGraphicsScene;
class NodeGraphModel;

namespace Ui {
class MainWindow;
}

class MainWindow :
	public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void showOpenFileDialog();
	
private:
	Ui::MainWindow *ui;
	QGraphicsScene *m_scene;
	NodeGraphModel *m_model;
};

#endif // H_MAINWINDOW
