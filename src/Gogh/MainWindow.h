#ifndef H_MAINWINDOW
#define H_MAINWINDOW

#include <QMainWindow>

class NodeGraphScene;
class NodeGraphModel;
class EnvModel;

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
	QString filename;

private slots:
	void showOpenFileDialog();
	void save();
	void showSaveAsFileDialog();
	void showEnvDialog();

private:
	Ui::MainWindow *ui;
	NodeGraphScene *m_scene;
	NodeGraphModel *m_model;
	EnvModel *m_envModel;
};

#endif // H_MAINWINDOW
