#ifndef H_RENDERDIALOG
#define H_RENDERDIALOG

#include <QDialog>
#include <string>

class QLabel;
class QScrollArea;
class QProcess;

class RenderDialog : public QDialog
{
	Q_OBJECT

public:
	RenderDialog(std::vector<std::string> cmd, QWidget *parent = nullptr);
	~RenderDialog();

protected:
	void closeEvent(QCloseEvent *event) override;
	void showEvent(QShowEvent *event) override;

private:
	void setRunning(bool running);

private slots:
	void onProcessFinished();
	void readStdout();
	void readStderr();
	void cancel();

private:
	bool m_isRunning;
	std::vector<std::string> m_cmd;
	QProcess *m_ffmpegProcess;
	QScrollArea *m_scrollArea;
	QLabel *m_processOutputLabel;
	QPushButton *m_closeButton;
	QPushButton *m_cancelButton;
};

#endif // H_RENDERDIALOG
