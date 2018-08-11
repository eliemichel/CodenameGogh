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
	explicit MainWindow(EnvModel *envModel, QString graphFilename = QString(), QWidget *parent = 0);
	~MainWindow();

	EnvModel *envModel() const { return m_envModel; }
	NodeGraphModel *model() const { return m_model; }

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
	QString m_currentFilename;
};

#endif // H_MAINWINDOW
