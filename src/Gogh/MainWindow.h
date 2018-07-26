#ifndef H_MAINWINDOW
#define H_MAINWINDOW

#include <QMainWindow>

class QGraphicsScene;

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
};

#endif // H_MAINWINDOW
